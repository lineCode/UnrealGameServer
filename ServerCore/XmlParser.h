#pragma once
#include "rapidxml.hpp"
using namespace rapidxml;

using XmlDocumentType = xml_document<WCHAR>;
using XmlNodeType = xml_node<WCHAR>;
using XmlAttributeType = xml_attribute<WCHAR>;

/*-----------------------------------------------------------------------------
이름     : XmlNode
용도     : rapidxml의 node를 편리하게 읽도록 돕는 객체
수정자   : 이민규
수정날짜 : 2022.10.08
------------------------------------------------------------------------------*/
class XmlNode
{
public:
	XmlNode(XmlNodeType* node = nullptr) : _node(node) { }

	bool				IsValid() { return _node != nullptr; }

	bool				GetBoolAttr(const WCHAR* key, bool defaultValue = false);
	int8				GetInt8Attr(const WCHAR* key, int8 defaultValue = 0);
	int16				GetInt16Attr(const WCHAR* key, int16 defaultValue = 0);
	int32				GetInt32Attr(const WCHAR* key, int32 defaultValue = 0);
	int64				GetInt64Attr(const WCHAR* key, int64 defaultValue = 0);
	float				GetFloatAttr(const WCHAR* key, float defaultValue = 0.0f);
	double				GetDoubleAttr(const WCHAR* key, double defaultValue = 0.0);
	const WCHAR* GetWStringAttr(const WCHAR* key, const WCHAR* defaultValue = L"");

	bool				GetBoolValue(bool defaultValue = false);
	int8				GetInt8Value(int8 defaultValue = 0);
	int16				GetInt16Value(int16 defaultValue = 0);
	int32				GetInt32Value(int32 defaultValue = 0);
	int64				GetInt64Value(int64 defaultValue = 0);
	float				GetFloatValue(float defaultValue = 0.0f);
	double				GetDoubleValue(double defaultValue = 0.0);
	const WCHAR* GetStringValue(const WCHAR* defaultValue = L"");

	XmlNode				FindChild(const WCHAR* key);
	Gvector<XmlNode>		FindChildren(const WCHAR* key);

private:
	XmlNodeType* _node = nullptr;
};

/*-----------------------------------------------------------------------------
이름     : XmlParser
용도     : Xml을 읽어오는 객체
수정자   : 이민규
수정날짜 : 2022.10.08
------------------------------------------------------------------------------*/
class XmlParser
{
public:
	bool ParseFromFile(const WCHAR* path, OUT XmlNode& root);

private:
	shared_ptr<XmlDocumentType>		_document = nullptr;
	GWString						_data;
};