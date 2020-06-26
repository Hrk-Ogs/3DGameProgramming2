#include "../../GameSystem.h"

#include "CollisionManager.h"
#include "CollisionFunctions.h"


//===============================================
//
// �R���C�_�[���m�̔���֐�
//
//===============================================

bool CollisionFunctions::SphereToSphere(SphereColliderComponent& sph1, SphereColliderComponent& sph2)
{
	// �� vs ��
	KdVec3 vDir = sph2.GetWorldSphere().Center - sph1.GetWorldSphere().Center;
	float dist = vDir.Length();

	// �������ĂȂ��Ȃ�I��
	if (dist >= sph1.GetWorldSphere().Radius + sph2.GetWorldSphere().Radius)
	{
		return false;
	}

	vDir.Normalize();

	// ���P���Ɍ��ʃf�[�^�ǉ�
	if (sph1.IsDoJudge())
	{
		// ���ʔ�v�Z
		float ratio = sph1.CalcMassRatio(sph2.GetOwner()->GetMass());
		// 
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(sph2.shared_from_this());		// ����̃R���C�_�[
		res.Dist = (dist - sph2.GetWorldSphere().Radius);											// �ŒZ����
		res.HitPosList.push_back(sph1.GetWorldSphere().Center + vDir * res.Dist);
		res.Push = -vDir * ((sph2.GetWorldSphere().Radius + sph1.GetWorldSphere().Radius) - dist) * ratio;	// �����x�N�g��
		sph1.AddResult(res);

		// �����߂�����
		if (sph1.IsTrigger() == false && sph2.IsTrigger() == false)
		{
			KdVec3 pos = sph1.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			sph1.GetOwner()->Transform()->SetPosition(pos);

			sph1.MoveSphere(res.Push);
		}

	}
	// ���Q���Ɍ��ʃf�[�^�ǉ�
	if (sph2.IsDoJudge())
	{
		// ���ʔ�v�Z
		float ratio = sph2.CalcMassRatio(sph1.GetOwner()->GetMass());
		// 
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(sph1.shared_from_this());		// ����̃R���C�_�[
		res.Dist = (dist - sph1.GetWorldSphere().Radius);											// �ŒZ����
		res.HitPosList.push_back(sph2.GetWorldSphere().Center - vDir * res.Dist);
		res.Push = vDir * ((sph2.GetWorldSphere().Radius + sph1.GetWorldSphere().Radius) - dist) * ratio;	// �����x�N�g��
		sph2.AddResult(res);

		// �����߂�����
		if (sph2.IsTrigger() == false && sph1.IsTrigger() == false)
		{
			KdVec3 pos = sph2.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			sph2.GetOwner()->Transform()->SetPosition(pos);

			sph2.MoveSphere(res.Push);
		}
	}

	return true;
}

bool CollisionFunctions::SphereToRay(SphereColliderComponent& sph, RayColliderComponent& ray)
{
	if (ray.GetRayLen() == 0)return false;

	// ���C��������������
	float dist;

	// ����
	if (sph.GetWorldSphere().Intersects(ray.GetRayPos(), ray.GetRayDir(), dist) == false) {
		return false;
	}

	// �����`�F�b�N
	if (dist > ray.GetRayLen())return false;

	// �����Ɍ��ʃf�[�^�ǉ�
	if (sph.IsDoJudge()) {
		// ���ʔ�v�Z
		float ratio = sph.CalcMassRatio(ray.GetOwner()->GetMass());
		// 
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(ray.shared_from_this());		// ����̃R���C�_�[
		res.Dist = dist;
		res.HitPosList.push_back(ray.GetRayPos() + ray.GetRayDir() * dist);
		res.Push = ray.GetRayDir() * (ray.GetRayLen() - dist) * ratio;
		sph.AddResult(res);

		// �����߂�����
		if (sph.IsTrigger() == false && ray.IsTrigger() == false)
		{
			KdVec3 pos = sph.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			sph.GetOwner()->Transform()->SetPosition(pos);

			sph.MoveSphere(res.Push);
		}
	}

	// ���C���Ɍ��ʃf�[�^�ǉ�
	if (ray.IsDoJudge()) {
		// ���ʔ�v�Z
		float ratio = ray.CalcMassRatio(sph.GetOwner()->GetMass());
		// 
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(sph.shared_from_this());		// ����̃R���C�_�[
		res.Dist = dist;
		res.HitPosList.push_back(ray.GetRayPos() + ray.GetRayDir() * dist);
		res.Push = -ray.GetRayDir() * (ray.GetRayLen() - dist) * ratio;
		ray.AddResult(res);

		// �����߂�����
		if (ray.IsTrigger() == false && sph.IsTrigger() == false)
		{
			KdVec3 pos = ray.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			ray.GetOwner()->Transform()->SetPosition(pos);

			ray.MoveRay(res.Push);
		}
	}
	return true;
}

bool CollisionFunctions::SphereToBox(SphereColliderComponent& sph, BoxColliderComponent& box)
{
	// ���Ɣ��̔���
	KdVec3 vNearPt;
	KdPointToBox((KdVec3&)sph.GetWorldSphere().Center, box.GetWorldBox(), vNearPt);

	KdVec3 vD = vNearPt - sph.GetWorldSphere().Center;

	float len = vD.Length();
	// �������Ă��Ȃ��Ȃ�I��
	if (len >= sph.GetWorldSphere().Radius)return false;

	vD.Normalize();

	// �����Ɍ��ʃf�[�^�ǉ�
	if (sph.IsDoJudge()) {
		// ���ʔ�v�Z
		float ratio = sph.CalcMassRatio(box.GetOwner()->GetMass());
		//
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(box.shared_from_this());		// ����̃R���C�_�[
		res.Dist = len;
		res.HitPosList.push_back(vNearPt);
		res.Push = -vD * (sph.GetWorldSphere().Radius - len) * ratio;
		sph.AddResult(res);

		// �����߂�����
		if (sph.IsTrigger() == false && box.IsTrigger() == false)
		{
			KdVec3 pos = sph.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			sph.GetOwner()->Transform()->SetPosition(pos);

			sph.MoveSphere(res.Push);
		}
	}

	// �����Ɍ��ʃf�[�^�ǉ�
	if (box.IsDoJudge()) {
		// ���ʔ�v�Z
		float ratio = box.CalcMassRatio(sph.GetOwner()->GetMass());
		//
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(sph.shared_from_this());		// ����̃R���C�_�[
		res.Dist = len;
		res.HitPosList.push_back(vNearPt);
		res.Push = vD * (sph.GetWorldSphere().Radius - len) * ratio;
		box.AddResult(res);

		// �����߂�����
		if (box.IsTrigger() == false && sph.IsTrigger() == false)
		{
			KdVec3 pos = box.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			box.GetOwner()->Transform()->SetPosition(pos);

			box.MoveBox(res.Push);
		}
	}

	return true;
}

bool CollisionFunctions::SphereToMesh(SphereColliderComponent& sph, MeshColliderComponent& mesh)
{
	bool bHit = false;

	// ���f�[�^���R�s�[
	auto sphData = sph.GetWorldSphere();

	// ����ݒ�E���ʃf�[�^
	KdSphereHit hitInfo;
	// �S�Ă̖ʂ̃q�b�g���W���󂯂�Ȃ����́A�P�����󂯎��
	if (sph.IsMeshDetailHitPos() == false)
	{
		hitInfo.MaxHitPositions = 1;
	}

	// �����߂��v�Z���s���ꍇ�́A�����߂��ꂽ���W���󂯎��
	if (sph.IsTrigger() == false && mesh.IsTrigger() == false)
	{
		hitInfo.MovedSpherePos = (KdVec3*)&sphData.Center;
	}

	//---------------------------------
	// �S���b�V���Ɣ���
	//---------------------------------
	auto& model = mesh.GetModel();
	for (auto&& meshIdx : model->GetNodeOutlinear().GetMeshNodeIndices())
	{
		auto& node = model->GetNodeOutlinear().GetAllNodes()[meshIdx];

		// �� vs ���b�V��
		if (KdSphereToMesh(	sphData.Center, sphData.Radius,								// ���̏��
							*node.Mesh, node.LocalTransform * mesh.GetMeshTransform(),	// ���b�V���̏��
							hitInfo
		))
		{
			bHit = true;
		}
	}

	if (bHit == false)
	{
		// �������ĂȂ��Ȃ�I��
		return false;
	}

	KdVec3 vPush = sphData.Center - sph.GetWorldSphere().Center;

	// �����Ɍ��ʃf�[�^�ǉ�
	if (sph.IsDoJudge())
	{
		// ���ʔ�v�Z
		float ratio = sph.CalcMassRatio(mesh.GetOwner()->GetMass());
		// 
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(mesh.shared_from_this());		// ����̃R���C�_�[
		res.Push = vPush * ratio;					// �����߂��x�N�g��
		res.HitPosList = hitInfo.HitPositions;
		sph.AddResult(res);

		// �����߂�����
		if (sph.IsTrigger() == false && mesh.IsTrigger() == false)
		{
			KdVec3 pos = sph.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			sph.GetOwner()->Transform()->SetPosition(pos);

			sph.MoveSphere(res.Push);
		}
	}

	// ���b�V�����Ɍ��ʃf�[�^�ǉ�
	if (mesh.IsDoJudge())
	{
		// ���ʔ�v�Z
		float ratio = mesh.CalcMassRatio(sph.GetOwner()->GetMass());
		//
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(sph.shared_from_this());		// ����̃R���C�_�[
		res.Push = -vPush * ratio;
		res.HitPosList = hitInfo.HitPositions;
		mesh.AddResult(res);

		// �����߂�����
		if (mesh.IsTrigger() == false && sph.IsTrigger() == false)
		{
			KdVec3 pos = mesh.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			mesh.GetOwner()->Transform()->SetPosition(pos);

			mesh.Move(res.Push);
		}
	}

	return true;
}

bool CollisionFunctions::RayToBox(RayColliderComponent& ray, BoxColliderComponent& box)
{
	if (ray.GetRayLen() == 0)return false;

	// ���C vs ��
	float dist;
	if (!box.GetWorldBox().Intersects(ray.GetRayPos(), ray.GetRayDir(), dist))return false;

	if (dist < 0 || dist > ray.GetRayLen())return false;

	// ���C���Ɍ��ʃf�[�^�ǉ�
	if (ray.IsDoJudge()) {
		// ���ʔ�v�Z
		float ratio = ray.CalcMassRatio(box.GetOwner()->GetMass());
		//
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(box.shared_from_this());		// ����̃R���C�_�[
		res.Dist = dist;													// ����
		res.HitPosList.push_back(ray.GetRayPos() + ray.GetRayDir() * dist);	// �ʒu
		res.Push = -ray.GetRayDir() * (ray.GetRayLen() - dist) * ratio;
		ray.AddResult(res);

		// �����߂�����
		if (ray.IsTrigger() == false && box.IsTrigger() == false)
		{
			KdVec3 pos = ray.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			ray.GetOwner()->Transform()->SetPosition(pos);

			ray.MoveRay(res.Push);
		}
	}

	// �����Ɍ��ʃf�[�^�ǉ�
	if (box.IsDoJudge()) {
		// ���ʔ�v�Z
		float ratio = box.CalcMassRatio(ray.GetOwner()->GetMass());
		//
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(ray.shared_from_this());		// ����̃R���C�_�[
		res.Dist = dist;
		res.HitPosList.push_back(ray.GetRayPos() + ray.GetRayDir() * dist);
		res.Push = ray.GetRayDir() * (ray.GetRayLen() - dist) * ratio;
		box.AddResult(res);

		// �����߂�����
		if (box.IsTrigger() == false && ray.IsTrigger() == false)
		{
			KdVec3 pos = box.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			box.GetOwner()->Transform()->SetPosition(pos);

			box.MoveBox(res.Push);
		}
	}

	return true;
}

bool CollisionFunctions::RayToMesh(RayColliderComponent& ray, MeshColliderComponent& mesh)
{
	if (ray.GetRayLen() == 0)return false;

	// �q�b�g���
	bool bHit = false;

	float closestDist = FLT_MAX;
	KdSptr<KdMesh> closestMesh;
	UINT closestMeshFaceIdx = 0;

	KdRayHit rayHitInfo;
	std::list<KdVec3> hitPosList;

	// �S���b�V���Ɣ���
	auto& model = mesh.GetModel();
	for (auto&& meshIdx : model->GetNodeOutlinear().GetMeshNodeIndices())
	{
		auto& node = model->GetNodeOutlinear().GetAllNodes()[meshIdx];

		// ���C vs ���b�V��
		if (KdRayToMesh(ray.GetRayPos(), ray.GetRayDir(), ray.GetRayLen(), *node.Mesh, node.LocalTransform * mesh.GetMeshTransform(), rayHitInfo))
		{
			// ��ԋ߂��H
			if (rayHitInfo.Distance < closestDist)
			{
				// ���̏��ōX�V
				closestDist = rayHitInfo.Distance;
				closestMeshFaceIdx = rayHitInfo.FaceIdx;
				closestMesh = node.Mesh;

				bHit = true;
			}

			// �q�b�g�ʒu�v�Z
			hitPosList.push_back(ray.GetRayPos() + ray.GetRayDir() * rayHitInfo.Distance);
		}
	}

	// �Ȃ�ɂ��������ĂȂ��Ȃ�I��
	if (bHit == false)return false;

	// ���C�̒������Z���Ȃ�A�q�b�g���Ă��Ȃ��Ƃ���
	if (closestDist > ray.GetRayLen())return false;

	// ���C���Ɍ��ʃf�[�^�ǉ�
	if (ray.IsDoJudge())
	{
		// ���ʔ�v�Z
		float ratio = ray.CalcMassRatio(mesh.GetOwner() ? mesh.GetOwner()->GetMass() : 0);
		//
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(mesh.shared_from_this());	// ����̃R���C�_�[
		res.Dist = closestDist;																	// ����
		res.HitPosList = hitPosList;															// �q�b�g�ʒu���X�g
		res.Mesh = closestMesh;
		res.Mesh_FaceIdx = closestMeshFaceIdx;													// �ʂ̃C���f�b�N�X
		res.Push = -ray.GetRayDir() * (ray.GetRayLen() - closestDist) * ratio;
		ray.AddResult(res);

		// �����߂�����
		if (ray.IsTrigger() == false && mesh.IsTrigger() == false)
		{
			KdVec3 pos = ray.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			ray.GetOwner()->Transform()->SetPosition(pos);

			ray.MoveRay(res.Push);
		}
	}

	// ���b�V���Ɍ��ʃf�[�^�ǉ�
	if (mesh.IsDoJudge())
	{
		// ���ʔ�v�Z
		float ratio = mesh.CalcMassRatio(ray.GetOwner() ? ray.GetOwner()->GetMass() : 0);
		//
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(ray.shared_from_this());		// ����̃R���C�_�[
		res.Dist = closestDist; 
		res.HitPosList = hitPosList;															// �q�b�g�ʒu���X�g
		res.Push = ray.GetRayDir() * (ray.GetRayLen() - closestDist) * ratio;
		mesh.AddResult(res);

		// �����߂�����
		if (mesh.IsTrigger() == false && ray.IsTrigger() == false)
		{
			KdVec3 pos = mesh.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			mesh.GetOwner()->Transform()->SetPosition(pos);

			mesh.Move(res.Push);
		}
	}

	return true;
}

bool CollisionFunctions::BoxToBox(BoxColliderComponent& box1, BoxColliderComponent& box2)
{
	// ����@�����̊֐��ł́A���m�ȃq�b�g�ʒu�����߂邱�Ƃ͏o���Ȃ�
	if (!box1.GetWorldBox().Intersects(box2.GetWorldBox()))
	{
		return false;
	}

	// ���P���Ɍ��ʃf�[�^�ǉ�
	if (box1.IsDoJudge())
	{
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(box2.shared_from_this());		// ����̃R���C�_�[
		res.HitPosList.push_back(box2.GetWorldBox().Center);
		box1.AddResult(res);
	}

	// ���Q���Ɍ��ʃf�[�^�ǉ�
	if (box2.IsDoJudge())
	{
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(box1.shared_from_this());		// ����̃R���C�_�[
		res.HitPosList.push_back(box1.GetWorldBox().Center);
		box2.AddResult(res);
	}

	return true;
}
