#include "../GameSystem.h"

#include "LightComponent.h"

//====================================================================
// カメラの情報から、ぴったり収まるビュー行列と正射影行列を求める
// mView : カメラのビュー行列
// mProj : カメラの射影行列
// lightDir : ライトの方向
// outView : [出力]算出したビュー行列
// outProj : [出力]算出した射影行列
//====================================================================
static void CreateLightViewProj(const KdMatrix& mView, const KdMatrix& mProj, const KdVec3& lightDir, KdMatrix& outView, KdMatrix& outProj)
{
	//-------------------------------------------
	// 現在の視錐台の各点(8座標)を求める
	// 射影座標系からワールド座標系へ逆変換
	//-------------------------------------------
	KdMatrix camInvProj = mProj;
	camInvProj.Inverse();
	KdMatrix mCam = mView;
	mCam.Inverse();
	KdMatrix camInvPV = camInvProj * mCam;

	// -1～1(Zは0～1)の射影座標系のBOXデータ
	std::vector<KdVec3> PtTbl;
	PtTbl.push_back(KdVec3(1, 1, 0));
	PtTbl.push_back(KdVec3(-1, 1, 0));
	PtTbl.push_back(KdVec3(-1, -1, 0));
	PtTbl.push_back(KdVec3(1, -1, 0));
	PtTbl.push_back(KdVec3(1, 1, 1));
	PtTbl.push_back(KdVec3(-1, 1, 1));
	PtTbl.push_back(KdVec3(-1, -1, 1));
	PtTbl.push_back(KdVec3(1, -1, 1));
	// 射影座標系の視錐台８座標をワールド座標系へ変換
	for (UINT i = 0; i < PtTbl.size(); i++) {
		PtTbl[i].TransformCoord(camInvPV);
	}
	// 仮のビュー行列作成(座標はまだ未定なので方向のみ)
	KdMatrix lightView;
	lightView.LookTo(lightDir, { 0, 1, 0 });
	lightView.Inverse();

	// 視錐台の８点の座標を仮のビュー座標系へ変換
	for (UINT i = 0; i < PtTbl.size(); i++) {
		PtTbl[i].TransformCoord(lightView);
	}
	// 視錐台のAABBを算出
	KdVec3 vMin, vMax;
	vMin = PtTbl[0];
	vMax = PtTbl[0];
	for (UINT i = 1; i < PtTbl.size(); i++) {
		if (vMin.x > PtTbl[i].x)vMin.x = PtTbl[i].x;
		if (vMin.y > PtTbl[i].y)vMin.y = PtTbl[i].y;
		if (vMin.z > PtTbl[i].z)vMin.z = PtTbl[i].z;
		if (vMax.x < PtTbl[i].x)vMax.x = PtTbl[i].x;
		if (vMax.y < PtTbl[i].y)vMax.y = PtTbl[i].y;
		if (vMax.z < PtTbl[i].z)vMax.z = PtTbl[i].z;
	}

	// AABBから、ぴったり収まる大きさの正射影行列作成
	outProj.CreateProjection_Orthographic(vMax.x - vMin.x, vMax.y - vMin.y, 0, vMax.z - vMin.z);
	// AABBの中心(XとY)のビュー行列を作成(ただしZ=0にする)
	KdVec3 vCenter = vMin + (vMax - vMin) * 0.5f;
	vCenter.z = vMin.z;
	lightView.Inverse();
	vCenter.TransformCoord(lightView);
	outView.CreateTranslation(vCenter);
	outView.LookTo(lightDir, { 0, 1, 0 });
	outView.Inverse();
}

void LightComponent::GenerateShadowMap(std::function<void()> proc)
{
	if (m_lightType == LightTypes::Spot)
	{
		
		// 記憶
		ComPtr<ID3D11RenderTargetView> saveRTV;
		ComPtr<ID3D11DepthStencilView> saveDSV;
		D3D.GetDevContext()->OMGetRenderTargets(1, &saveRTV, &saveDSV);

		// 深度クリッピングを無効にする
		D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullBack_NoZClip);

		// レンダーターゲットとZバッファを変更
		SHADER.m_genShadowMapShader.SetRenderTarget_SpotShadowMap(m_myLightIndex);

		// ビュー行列
		KdMatrix mV = GetOwner()->Transform()->GetMatrix();
		mV.Inverse();
		// 射影行列
		KdMatrix mP;
		mP.CreateProjection_PerspectiveFov(m_spotAngle * KdToRadians, 1, 0.01f, m_range);
		SHADER.m_genShadowMapShader.SetLightVP(mV * mP);

		SHADER.m_cb8_Light.Work().SL[m_myLightIndex].mLightVP = mV * mP;
		SHADER.m_cb8_Light.Write();

		// 描画実行
		SHADER.m_genShadowMapShader.SetToDevice();
		proc();

		// いろいろ元に戻す
		D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullBack); // RS
		D3D.GetDevContext()->OMSetRenderTargets(1, saveRTV.GetAddressOf(), saveDSV.Get()); // RTとZバッファ
		D3D.SetViewportFromRTV(saveRTV.Get()); // ビューポート
	}
	else if (m_lightType == LightTypes::Directional)
	{
		// 記憶
		ComPtr<ID3D11RenderTargetView> saveRTV;
		ComPtr<ID3D11DepthStencilView> saveDSV;
		D3D.GetDevContext()->OMGetRenderTargets(1, &saveRTV, &saveDSV);

		// 深度クリッピングを無効にする
		D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullBack_NoZClip);

		for (int ci = 0; ci < m_cascadeNum; ci++)
		{
			//データチェック
			if (m_cascadeDist[ci] <= 0.0001f)m_cascadeDist[ci] = 0.0001f;

			// レンダーターゲットとZバッファを変更
			SHADER.m_genShadowMapShader.SetRenderTarget_DirShadowMap(m_myLightIndex, ci);

			// 現在カメラの射影行列から、視野角、アスペクト比、最近接距離など算出する
			const KdMatrix& camP = SHADER.m_cb7_Camera.GetWork().mP;
			float aspect = camP._22 / camP._11; // アスペクト比
			float fNear = camP._43 / -camP._33; // Nearクリップ
			float fov = 2 * atan(1.0f / camP._22); // 視野角
			// １番目以降は、1つ前の距離を使用
			if (ci >= 1)
			{
				fNear = m_cascadeDist[ci - 1];
			}
			// Farの方が小さいとまずい
			if (fNear >= m_cascadeDist[ci])m_cascadeDist[ci] = fNear + 0.01f;

			KdMatrix mTmpProj;
			// fNear ～ 50mまでの射影行列を作成
			mTmpProj.CreateProjection_PerspectiveFov(fov, aspect, fNear, m_cascadeDist[ci]);

			// このカメラ範囲を完全に覆い囲むビュー行列と正射影行列を求める
			KdMatrix mV, mP;
			CreateLightViewProj(SHADER.m_cb7_Camera.GetWork().mV, mTmpProj, GetOwner()->Transform()->GetRotation().Forward(), mV, mP);

			SHADER.m_genShadowMapShader.SetLightVP(mV * mP);

			SHADER.m_cb8_Light.Work().DL[m_myLightIndex].CascadeDist = m_cascadeDist;
			SHADER.m_cb8_Light.Work().DL[m_myLightIndex].CascadeNum = m_cascadeNum;
			SHADER.m_cb8_Light.Work().DL[m_myLightIndex].mLightVP[ci] = mV * mP;
			SHADER.m_cb8_Light.Write();

			// 描画実行
			SHADER.m_genShadowMapShader.SetToDevice();
			proc();
		}

		// いろいろ元に戻す
		D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullBack); // RS
		D3D.GetDevContext()->OMSetRenderTargets(1, saveRTV.GetAddressOf(), saveDSV.Get()); // RTとZバッファ
		D3D.SetViewportFromRTV(saveRTV.Get()); // ビューポート
	}
}

void LightComponent::PrepareDraw(RenderingData& rdata)
{
	if (m_enable == false)return;

	m_myLightIndex = -1;

	//-----------------
	// ライトデータ登録
	//-----------------
	// ディレクショナルライト
	if (m_lightType == LightTypes::Directional) {
		// 現在の登録数
		int idx = SHADER.m_cb8_Light.Work().DL_Cnt;
		if (idx < (int)SHADER.m_cb8_Light.Work().DL.size()) {
			m_myLightIndex = idx;	// 自分のライト番号

			KdMatrix m = GetOwner()->Transform()->GetMatrix();

			// ライトデータを定数バッファへ書き込む
			SHADER.m_cb8_Light.Work().DL[idx].Dir = m.Forward();
			SHADER.m_cb8_Light.Work().DL[idx].Dir.Normalize();
			SHADER.m_cb8_Light.Work().DL[idx].Color = m_color * m_intensity;
			SHADER.m_cb8_Light.Work().DL[idx].EnableShadow = m_isGenerateShadowMap ? 1.0f : 0.0f;

			// ライト数を加算
			SHADER.m_cb8_Light.Work().DL_Cnt++;

			// シャドウマップ登録
			if (m_isGenerateShadowMap)
			{
				rdata.m_shadowMapLight.push_back(this);
			}
		}
	}
	// ポイントライト
	else if (m_lightType == LightTypes::Point) {
		KdMatrix m = GetOwner()->Transform()->GetMatrix();

		// 現在の登録数
		int idx = SHADER.m_cb8_Light.Work().PL_Cnt;
		if (idx < (int)SHADER.m_cb8_Light.Work().PL.size())	// 限界数を超えている場合は、このライトは使用しない
		{
			m_myLightIndex = idx;

			// ライトデータを定数バッファへ書き込む
			SHADER.m_cb8_Light.Work().PL[idx].Pos = m.Translation();
			SHADER.m_cb8_Light.Work().PL[idx].Radius = m_range;
			SHADER.m_cb8_Light.Work().PL[idx].Color = m_color * m_intensity;

			// ライト数を加算
			SHADER.m_cb8_Light.Work().PL_Cnt++;
		}
	}
	// スポットライト
	else if(m_lightType==LightTypes::Spot){
		KdMatrix m = GetOwner()->Transform()->GetMatrix();

		// 現在の登録数
		int idx = SHADER.m_cb8_Light.Work().SL_Cnt;
		if (idx < (int)SHADER.m_cb8_Light.Work().SL.size()) {
			m_myLightIndex = idx;		// 自分のライト番号

			// ライトデータを定数バッファへ書き込む
			SHADER.m_cb8_Light.Work().SL[idx].Pos = m.Translation();
			SHADER.m_cb8_Light.Work().SL[idx].Dir = m.Forward();
			SHADER.m_cb8_Light.Work().SL[idx].Dir.Normalize();
			SHADER.m_cb8_Light.Work().SL[idx].Range = m_range;
			SHADER.m_cb8_Light.Work().SL[idx].Color = m_color * m_intensity;
			SHADER.m_cb8_Light.Work().SL[idx].OuterCorn = m_spotAngle * KdToRadians;
			SHADER.m_cb8_Light.Work().SL[idx].EnableShadow = m_isGenerateShadowMap ? 1.0f : 0.0f;

			// ライト数を加算
			SHADER.m_cb8_Light.Work().SL_Cnt++;

			// シャドウマップ登録
			if (m_isGenerateShadowMap)
			{
				rdata.m_shadowMapLight.push_back(this);
			}
		}

	}
}

void LightComponent::Editor_ImGuiUpdate()
{
	// 継承前の処理も実行
	BaseComponent::Editor_ImGuiUpdate();

	// ライトの種類
	std::vector<char*> lightTypes = { u8"Directional",u8"Point",u8"Spot" };
	ImGui::Combo(u8"種類", (int*)&m_lightType, &lightTypes[0], lightTypes.size());
	// 色
	ImGui::ColorEdit3(u8"Color", &m_color.x);
	// 強度
	ImGui::DragFloat(u8"Intensity", &m_intensity, 0.01f, 0, 100);

	// 範囲（点光と集中光のみの設定)
	if (m_lightType == LightTypes::Point || m_lightType == LightTypes::Spot) {
		ImGui::DragFloat(u8"Range", &m_range, 0.01f, 0, 10000);
	}
	// 角度（集中光のみの設定）
	if (m_lightType == LightTypes::Spot) {
		ImGui::DragFloat(u8"Angle", &m_spotAngle, 0, 0, 360);
	}

	// 影生成
	ImGui::Checkbox(u8"影生成", &m_isGenerateShadowMap);

	if (m_isGenerateShadowMap)
	{
		// 平行光の時はカスケード設定
		if (m_lightType == LightTypes::Directional)
		{
			ImGui::SliderInt(u8"カスケード分割数", &m_cascadeNum, 1, 4);
			ImGui::DragFloat4(u8"カスケード分割距離", &m_cascadeDist.x, 0.01f, 0.01f, 10000);
		}
	}

	// シャドウマップ画像
	if (m_lightType == LightTypes::Directional && m_isGenerateShadowMap)
	{
		ImGui::Image((ImTextureID)SHADER.m_genShadowMapShader.GetDirShadowMap()->GetSRView(m_myLightIndex * 4 + 0), ImVec2(200, 200));
		ImGui::Image((ImTextureID)SHADER.m_genShadowMapShader.GetDirShadowMap()->GetSRView(m_myLightIndex * 4 + 1), ImVec2(200, 200));
		ImGui::Image((ImTextureID)SHADER.m_genShadowMapShader.GetDirShadowMap()->GetSRView(m_myLightIndex * 4 + 2), ImVec2(200, 200));
		ImGui::Image((ImTextureID)SHADER.m_genShadowMapShader.GetDirShadowMap()->GetSRView(m_myLightIndex * 4 + 3), ImVec2(200, 200));
	}

	//================
	// デバッグ線
	//================
	// ディレクショナルライト
	KdMatrix m = GetOwner()->Transform()->GetMatrix();
	if (m_lightType == LightTypes::Directional) {
		GAMESYS.DebugLine().AddLine(m.Translation(), m.Translation() + m.Forward() * 0.5f, { 1,1,0,1 });
		m = KdMatrix().CreateTranslation(0.1f, 0, 0) * m;
		GAMESYS.DebugLine().AddLine(m.Translation(), m.Translation() + m.Forward() * 0.5f, { 1,1,0,1 });
		m = KdMatrix().CreateTranslation(-0.2f, 0, 0) * m;
		GAMESYS.DebugLine().AddLine(m.Translation(), m.Translation() + m.Forward() * 0.5f, { 1,1,0,1 });
	}
	else if (m_lightType == LightTypes::Point) {
		// デバッグ線
		GAMESYS.DebugLine().AddSphereLine(m.Translation(), m_range, m_color.ToVec4(0.5f));
	}

}

