#pragma once

// JSON�ǂݍ���
inline json11::Json KdLoadJSONFile(const std::string& filename)
{
	// JSON�t�@�C�����J��
	std::ifstream ifs(filename);
	if (!ifs)return nullptr;

	// ������Ƃ��đS�ǂ݂�k��
	std::string strJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	// �������JSON�����(�p�[�X)����
	std::string err;
	json11::Json jsonObj = json11::Json::parse(strJson, err);
	if (err.size() > 0)return nullptr;

	return jsonObj;
}

// obj��bool�^�̏ꍇ�Ɍ���擾���Aresult�ɓ����
inline void KdJsonGet(const json11::Json& obj, bool& result)
{
	// �w��^�łȂ��ꍇ�̓X�L�b�v
	if (obj.is_bool() == false)return;

	result = obj.bool_value();
}

// obj��string�^�̏ꍇ�Ɍ���擾���Aresult�ɓ����
inline void KdJsonGet(const json11::Json& obj, std::string& result)
{
	// �w��^�łȂ��ꍇ�̓X�L�b�v
	if (obj.is_string() == false)return;

	result = obj.string_value();
}

// obj�����W�̏ꍇ�Ɍ���擾���Aresult�ɓ����
inline void KdJsonGet(const json11::Json& obj, KdVec3& result)
{
	// �w��^�łȂ��ꍇ�̓X�L�b�v
	if (obj.is_array() == false)return;
	if (obj.array_items().size() != 3)return;

	result.x = (float)obj.array_items()[0].number_value();
	result.y = (float)obj.array_items()[1].number_value();
	result.z = (float)obj.array_items()[2].number_value();
}

// obj��KdModel�^�̏ꍇ�Ɍ���擾���Aresult�ɓ����
inline void KdJsonGet(const json11::Json& obj, KdSptr<KdModel>& result)
{
	// �w��^�łȂ��ꍇ�̓X�L�b�v
	if (obj.is_string() == false)return;

	result = KDResFactory.GetModel(obj.string_value());
}

// obj��float�^�̏ꍇ�Ɍ���擾���Aresult�ɓ����
inline void KdJsonGet(const json11::Json& obj, float& result)
{
	// �w��^�łȂ��ꍇ�̓X�L�b�v
	if (obj.is_number() == false)return;

	result = (float)obj.number_value();
}

// obj��Quaternion�^�̏ꍇ�Ɍ���擾���Aresult�ɓ����
inline void KdJsonGet(const json11::Json& obj, KdQuaternion& result)
{
	// �w��^�łȂ��ꍇ�̓X�L�b�v
	if (obj.is_array() == false)return;
	if (obj.array_items().size() != 4)return;

	result.x = (float)obj.array_items()[0].number_value();
	result.y = (float)obj.array_items()[1].number_value();
	result.z = (float)obj.array_items()[2].number_value();
	result.w = (float)obj.array_items()[3].number_value();
}

// obj��int�^�̏ꍇ�Ɍ���擾���Aresult�ɓ����
inline void KdJsonGet(const json11::Json& obj, int& result)
{
	// �w��^�łȂ��ꍇ�̓X�L�b�v
	if (obj.is_number() == false)return;

	result = obj.int_value();
}
