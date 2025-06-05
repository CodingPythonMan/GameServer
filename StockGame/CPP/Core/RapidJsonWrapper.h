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

    /// Ű�� �ش��ϴ� ���� �����ϴ��� �˻� (�ֻ��� ��ü ����)
    bool HasKey(const std::string& key) const;

    bool GetInt(const std::string& key, int& outValue) const;

    /// Ű�� �ش��ϴ� �Ǽ�(float) ��ȯ
    bool GetFloat(const std::string& key, float& outValue) const;

    /// Ű�� �ش��ϴ� ���ڿ�(std::string) ��ȯ
    bool GetString(const std::string& key, std::string& outValue) const;

    /// Ű�� �ش��ϴ� Boolean ��ȯ
    bool GetBool(const std::string& key, bool& outValue) const;

    /// Ű�� �ش��ϴ� �迭(Array) ũ�� ��ȯ
    int GetArraySize(const std::string& key) const;

    const rapidjson::Value* GetArrayElement(int index) const;

    /// ������ Ű �Ʒ��� �迭�� std::vector<int>�� ��ȯ
    bool GetIntArray(const std::string& key, std::vector<int>& outVec) const;

    /// ������ Ű �Ʒ��� �迭�� std::vector<float>�� ��ȯ
    bool GetFloatArray(const std::string& key, std::vector<float>& outVec) const;

    /// ������ Ű �Ʒ��� �迭�� std::vector<std::string>�� ��ȯ
    bool GetStringArray(const std::string& key, std::vector<std::string>& outVec) const;

    const rapidjson::Document& GetDocument() const;

private:
    rapidjson::Document mDocument;

    // JSON ������ �� ���� std::string���� ��ȯ
    bool _ReadFileToString(const std::string& filePath, std::string& out) const;
};