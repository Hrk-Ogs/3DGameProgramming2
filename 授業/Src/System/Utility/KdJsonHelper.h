#pragma once

// JSON読み込み
inline json11::Json KdLoadJSONFile(const std::string& filename)
{
	// JSONファイルを開く
	std::ifstream ifs(filename);
	if (!ifs)return nullptr;

	// 文字列として全読みおk無
	std::string strJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	// 文字列のJSONを解析(パース)する
	std::string err;
	json11::Json jsonObj = json11::Json::parse(strJson, err);
	if (err.size() > 0)return nullptr;

	return jsonObj;
}

// objをbool型の場合に限り取得し、resultに入れる
inline void KdJsonGet(const json11::Json& obj, bool& result)
{
	// 指定型でない場合はスキップ
	if (obj.is_bool() == false)return;

	result = obj.bool_value();
}

// objをstring型の場合に限り取得し、resultに入れる
inline void KdJsonGet(const json11::Json& obj, std::string& result)
{
	// 指定型でない場合はスキップ
	if (obj.is_string() == false)return;

	result = obj.string_value();
}

// objを座標の場合に限り取得し、resultに入れる
inline void KdJsonGet(const json11::Json& obj, KdVec3& result)
{
	// 指定型でない場合はスキップ
	if (obj.is_array() == false)return;
	if (obj.array_items().size() != 3)return;

	result.x = (float)obj.array_items()[0].number_value();
	result.y = (float)obj.array_items()[1].number_value();
	result.z = (float)obj.array_items()[2].number_value();
}

// objをKdModel型の場合に限り取得し、resultに入れる
inline void KdJsonGet(const json11::Json& obj, KdSptr<KdModel>& result)
{
	// 指定型でない場合はスキップ
	if (obj.is_string() == false)return;

	result = KDResFactory.GetModel(obj.string_value());
}

// objをfloat型の場合に限り取得し、resultに入れる
inline void KdJsonGet(const json11::Json& obj, float& result)
{
	// 指定型でない場合はスキップ
	if (obj.is_number() == false)return;

	result = (float)obj.number_value();
}

// objをQuaternion型の場合に限り取得し、resultに入れる
inline void KdJsonGet(const json11::Json& obj, KdQuaternion& result)
{
	// 指定型でない場合はスキップ
	if (obj.is_array() == false)return;
	if (obj.array_items().size() != 4)return;

	result.x = (float)obj.array_items()[0].number_value();
	result.y = (float)obj.array_items()[1].number_value();
	result.z = (float)obj.array_items()[2].number_value();
	result.w = (float)obj.array_items()[3].number_value();
}

// objをint型の場合に限り取得し、resultに入れる
inline void KdJsonGet(const json11::Json& obj, int& result)
{
	// 指定型でない場合はスキップ
	if (obj.is_number() == false)return;

	result = obj.int_value();
}
