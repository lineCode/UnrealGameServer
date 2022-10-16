#include "pch.h"
#include "XmlParser.h"
#include "FileUtils.h"

_locale_t kr = _create_locale(LC_NUMERIC, "kor");

/*-----------------------------------------------------------------------------
이름     : XmlParser::GetBoolAttr
용도     : key값에 맞는 Attribute(속성) 값을 bool형으로 변환 후 반환하는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
------------------------------------------------------------------------------*/
bool XmlNode::GetBoolAttr(const WCHAR* key, bool defaultValue)
{
	XmlAttributeType* attr = _node->first_attribute(key);
	if (attr)
		return ::_wcsicmp(attr->value(), L"true") == 0;

	return defaultValue; 
}

/*-----------------------------------------------------------------------------
이름     : XmlParser::GetInt8Attr
용도     : key값에 맞는 Attribute(속성) 값을 int8형으로 변환 후 반환하는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
------------------------------------------------------------------------------*/
int8 XmlNode::GetInt8Attr(const WCHAR* key, int8 defaultValue)
{
	XmlAttributeType* attr = _node->first_attribute(key);
	if (attr)
		return static_cast<int8>(::_wtoi(attr->value()));

	return defaultValue;
}

/*-----------------------------------------------------------------------------
이름     : XmlParser::GetInt16Attr
용도     : key값에 맞는 Attribute(속성) 값을 int16형으로 변환 후 반환하는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
------------------------------------------------------------------------------*/
int16 XmlNode::GetInt16Attr(const WCHAR* key, int16 defaultValue)
{
	XmlAttributeType* attr = _node->first_attribute(key);
	if (attr)
		return static_cast<int16>(::_wtoi(attr->value()));

	return defaultValue;
}

/*-----------------------------------------------------------------------------
이름     : XmlParser::GetInt32Attr
용도     : key값에 맞는 Attribute(속성) 값을 int32형으로 변환 후 반환하는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
------------------------------------------------------------------------------*/
int32 XmlNode::GetInt32Attr(const WCHAR* key, int32 defaultValue)
{
	XmlAttributeType* attr = _node->first_attribute(key);
	if (attr)
		return ::_wtoi(attr->value());

	return defaultValue;
}

/*-----------------------------------------------------------------------------
이름     : XmlParser::GetInt64Attr
용도     : key값에 맞는 Attribute(속성) 값을 int64형으로 변환 후 반환하는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
------------------------------------------------------------------------------*/
int64 XmlNode::GetInt64Attr(const WCHAR* key, int64 defaultValue)
{
	xml_attribute<WCHAR>* attr = _node->first_attribute(key);
	if (attr)
		return ::_wtoi64(attr->value());

	return defaultValue;
}

/*-----------------------------------------------------------------------------
이름     : XmlParser::GetFloatAttr
용도     : key값에 맞는 Attribute(속성) 값을 float형으로 변환 후 반환하는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
------------------------------------------------------------------------------*/
float XmlNode::GetFloatAttr(const WCHAR* key, float defaultValue)
{
	XmlAttributeType* attr = _node->first_attribute(key);
	if (attr)
		return static_cast<float>(::_wtof(attr->value()));

	return defaultValue;
}

/*-----------------------------------------------------------------------------
이름     : XmlParser::GetDoubleAttr
용도     : key값에 맞는 Attribute(속성) 값을 double형으로 변환 후 반환하는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
------------------------------------------------------------------------------*/
double XmlNode::GetDoubleAttr(const WCHAR* key, double defaultValue)
{
	XmlAttributeType* attr = _node->first_attribute(key);
	if (attr)
		return ::_wtof_l(attr->value(), kr);

	return defaultValue;
}

/*-----------------------------------------------------------------------------
이름     : XmlParser::GetWStringAttr
용도     : key값에 맞는 Attribute(속성) 값을 WCHAR형으로 변환 후 반환하는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
------------------------------------------------------------------------------*/
const WCHAR* XmlNode::GetWStringAttr(const WCHAR* key, const WCHAR* defaultValue)
{
	XmlAttributeType* attr = _node->first_attribute(key);
	if (attr)
		return attr->value();

	return defaultValue;
}

/*-----------------------------------------------------------------------------
이름     : XmlParser::Get( )Valye
용도     : 내가 원하는 Value 값을 원하는 자료형으로 변환하는 함수들
수정자   : 이민규
수정날짜 : 2022.10.08
------------------------------------------------------------------------------*/

bool XmlNode::GetBoolValue(bool defaultValue)
{
	WCHAR* val = _node->value();
	if (val)
		return ::_wcsicmp(val, L"true") == 0;

	return defaultValue;
}

int8 XmlNode::GetInt8Value(int8 defaultValue)
{
	WCHAR* val = _node->value();
	if (val)
		return static_cast<int8>(::_wtoi(val));

	return defaultValue;
}

int16 XmlNode::GetInt16Value(int16 defaultValue)
{
	WCHAR* val = _node->value();
	if (val)
		return static_cast<int16>(::_wtoi(val));
	return defaultValue;
}

int32 XmlNode::GetInt32Value(int32 defaultValue)
{
	WCHAR* val = _node->value();
	if (val)
		return static_cast<int32>(::_wtoi(val));

	return defaultValue;
}

int64 XmlNode::GetInt64Value(int64 defaultValue)
{
	WCHAR* val = _node->value();
	if (val)
		return static_cast<int64>(::_wtoi64(val));

	return defaultValue;
}

float XmlNode::GetFloatValue(float defaultValue)
{
	WCHAR* val = _node->value();
	if (val)
		return static_cast<float>(::_wtof(val));

	return defaultValue;
}

double XmlNode::GetDoubleValue(double defaultValue)
{
	WCHAR* val = _node->value();
	if (val)
		return ::_wtof_l(val, kr);

	return defaultValue;
}

const WCHAR* XmlNode::GetStringValue(const WCHAR* defaultValue)
{
	WCHAR* val = _node->first_node()->value();
	if (val)
		return val;

	return defaultValue;
}

/*-----------------------------------------------------------------------------
이름     : XmlParser::FindChild
용도     : Key 노드를 XmlNode형으로 가져오는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
------------------------------------------------------------------------------*/
XmlNode XmlNode::FindChild(const WCHAR* key)
{
	return XmlNode(_node->first_node(key));
}

/*-----------------------------------------------------------------------------
이름     : XmlParser::FindChildren
용도     : Key 노드를 XmlNode형으로 모두 가져오는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
------------------------------------------------------------------------------*/
Gvector<XmlNode> XmlNode::FindChildren(const WCHAR* key)
{
	Gvector<XmlNode> nodes;

	xml_node<WCHAR>* node = _node->first_node(key);
	while (node)
	{
		nodes.push_back(XmlNode(node));
		node = node->next_sibling(key);
	}

	return nodes;
}

/*-----------------------------------------------------------------------------
이름     : XmlParser::ParseFromFile
용도     : 경로에 파일을 UTF-16으로 전환 후 rapidxml로 읽어온 후
           root에 첫번째 노드를 저장해주는 함수
수정자   : 이민규
수정날짜 : 2022.10.08
------------------------------------------------------------------------------*/
bool XmlParser::ParseFromFile(const WCHAR* path, OUT XmlNode& root)
{
	Gvector<BYTE> bytes = FileUtils::ReadFile(path);
	_data = FileUtils::ConvertUTF16(string(bytes.begin(), bytes.end()));

	if (_data.empty())
		return false;

	_document = GMakeShared<XmlDocumentType>();
	_document->parse<0>(reinterpret_cast<WCHAR*>(&_data[0]));
	root = XmlNode(_document->first_node());
	return true;
}