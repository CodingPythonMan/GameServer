#pragma once
#include <vector>
#include <string>
#include "RapidJson/document.h"
#include "RapidJson/rapidjson.h"

class RapidJsonWrapper
{
public:
    bool Parse(const std::string& jsonStr);

    bool ParseFile(const std::string& filePath);

    bool IsObject() const;

    bool IsArray() const;

    /// 키에 해당하는 값이 존재하는지 검사 (최상위 객체 기준)
    bool HasKey(const std::string& key) const;

    bool GetInt(const std::string& key, int& outValue) const;

    /// 키에 해당하는 실수(float) 반환
    bool GetFloat(const std::string& key, float& outValue) const;

    /// 키에 해당하는 문자열(std::string) 반환
    bool GetString(const std::string& key, std::string& outValue) const;

    /// 키에 해당하는 Boolean 반환
    bool GetBool(const std::string& key, bool& outValue) const;

    /// 키에 해당하는 배열(Array) 크기 반환
    int GetArraySize(const std::string& key) const;

    const rapidjson::Value* GetArrayElement(int index) const;

    /// 지정된 키 아래의 배열을 std::vector<int>로 반환
    bool GetIntArray(const std::string& key, std::vector<int>& outVec) const;

    /// 지정된 키 아래의 배열을 std::vector<float>로 반환
    bool GetFloatArray(const std::string& key, std::vector<float>& outVec) const;

    /// 지정된 키 아래의 배열을 std::vector<std::string>로 반환
    bool GetStringArray(const std::string& key, std::vector<std::string>& outVec) const;

    const rapidjson::Document& GetDocument() const;

private:
    rapidjson::Document mDocument;

    // JSON 파일을 한 번에 std::string으로 반환
    bool _ReadFileToString(const std::string& filePath, std::string& out) const;
};