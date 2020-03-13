#ifndef __JSONGETMEMBER_H
#define __JSONGETMEMBER_H
#include <ArduinoJson.h>

template<typename T>
void JsonParse_Element(JsonObject& _jsondoc, const char* _propertyName, T& _var)
{
	_var = _jsondoc.getMember(_propertyName).as<T>();
}

template<typename T>
void JsonParse_Element(JsonObject& _jsondoc, const char* _propertyName, T* _var, size_t _len)
{
	auto str = _jsondoc.getMember(_propertyName).as<const char*>();
	if (str != NULL)
		memccpy(_var, str, 0, _len);
}
//
template<typename T>
void JsonParse_Element(JsonDocument& _jsondoc, const char* _propertyName, T& _var)
{
	_var = _jsondoc.getMember(_propertyName).as<T>();
}

template<typename T>
void JsonParse_Element(JsonDocument& _jsondoc, const char* _propertyName, T* _var, size_t _len)
{
	auto str = _jsondoc.getMember(_propertyName).as<const char*>();
	if (str != NULL)
		memccpy(_var, str, 0, _len);
}

template<typename T>
void JsonParse_Element(JsonArray& _arr, uint8_t _index, const char* _propertyName, T& _var)
{
	_var = _arr[_index].getMember(_propertyName).as<T>();
}

template<typename T>
void JsonParse_Element(JsonArray& _arr, uint8_t _index, const char* _propertyName, T* _var, size_t _len)
{
	auto str = _arr[_index].getMember(_propertyName).as<const char*>();
	if (str != NULL)
		memccpy(_var, str, 0, _len);
}
#endif // !__JSONGETMEMBER_H
