#include "RapidJsonWrapper.h"
#include "CoreGlobal.h"
#include "ConsoleLog.h"
#include <fstream>
#include <sstream>

bool RapidJsonWrapper::Parse(const std::string& jsonStr)
{
    mDocument.Parse(jsonStr.c_str());
    if (mDocument.HasParseError())
    {
        size_t errOffset = mDocument.GetErrorOffset();
        auto errCode = mDocument.GetParseError();

        GConsoleLogger->WriteStdOut(Color::RED, 
            L"[JsonWrapper] JSON 파싱 오류 (offset : %d): %d", errOffset, errCode);

        return false;
    }

    return true;
}

bool RapidJsonWrapper::ParseFile(const std::string& filePath)
{
    std::string content;
    if (!_ReadFileToString(filePath, content))
    {
        return false;
    }

    return Parse(content);
}

bool RapidJsonWrapper::IsObject() const
{
    return mDocument.IsObject();
}

bool RapidJsonWrapper::IsArray() const
{
    return mDocument.IsArray();
}

bool RapidJsonWrapper::HasKey(const std::string& key) const
{
    if (false == mDocument.IsObject())
    {
        return false;
    }

    return mDocument.HasMember(key.c_str());
}

bool RapidJsonWrapper::GetInt(const std::string& key, int& outValue) const
{
    if (false == HasKey(key))
    {
        return false;
    }

    const rapidjson::Value& value = mDocument[key.c_str()];
    if (false == value.IsInt())
    {
        return false;
    }
    outValue = value.GetInt();
    return true;
}

bool RapidJsonWrapper::GetFloat(const std::string& key, float& outValue) const
{
    if (false == HasKey(key))
    {
        return false;
    }

    const rapidjson::Value& value = mDocument[key.c_str()];
    if (false == value.IsNumber())
    {
        return false;
    }
    outValue = static_cast<float>(value.GetDouble());
    return true;
}

bool RapidJsonWrapper::GetString(const std::string& key, std::string& outValue) const
{
    if (false == HasKey(key))
    {
        return false;
    }

    const rapidjson::Value& value = mDocument[key.c_str()];
    if (false == value.IsString())
    {
        return false;
    }
    outValue = value.GetString();
    return true;
}

bool RapidJsonWrapper::GetBool(const std::string& key, bool& outValue) const
{
    if (false == HasKey(key))
    {
        return false;
    }

    const rapidjson::Value& value = mDocument[key.c_str()];
    if (false == value.IsBool())
    {
        return false;
    }
    outValue = value.GetBool();
    return true;
}

int RapidJsonWrapper::GetArraySize(const std::string& key) const
{
    if (false == HasKey(key))
    {
        return -1;
    }

    const rapidjson::Value& value = mDocument[key.c_str()];
    if (false == value.IsArray())
    {
        return -1;
    }

    return static_cast<int>(value.Size());
}

const rapidjson::Value* RapidJsonWrapper::GetArrayElement(int index) const
{
    if (false == mDocument.IsArray())
    {
        return nullptr;
    }

    const rapidjson::Value& value = mDocument;
    if (index < 0 || index >= static_cast<int>(value.Size()))
    {
        return nullptr;
    }
    return &value[index];
}

bool RapidJsonWrapper::GetIntArray(const std::string& key, std::vector<int>& outVec) const
{
    if (false == HasKey(key))
    {
        return false;
    }

    const rapidjson::Value& arr = mDocument[key.c_str()];
    if (false == arr.IsArray())
    {
        return false;
    }
    outVec.clear();
    for (rapidjson::SizeType i = 0; i < arr.Size(); i++)
    {
        if (true == arr[i].IsInt())
        {
            outVec.push_back(arr[i].GetInt());
        }
    }

    return true;
}

bool RapidJsonWrapper::GetFloatArray(const std::string& key, std::vector<float>& outVec) const
{
    if (false == HasKey(key))
    {
        return false;
    }

    const rapidjson::Value& arr = mDocument[key.c_str()];
    if (false == arr.IsArray())
    {
        return false;
    }
    outVec.clear();
    for (rapidjson::SizeType i = 0; i < arr.Size(); i++)
    {
        if (true == arr[i].IsNumber())
        {
            outVec.push_back(static_cast<float>(arr[i].GetDouble()));
        }
    }

    return true;
}

bool RapidJsonWrapper::GetStringArray(const std::string& key, std::vector<std::string>& outVec) const
{
    if (false == HasKey(key))
    {
        return false;
    }

    const rapidjson::Value& arr = mDocument[key.c_str()];
    if (false == arr.IsArray())
    {
        return false;
    }
    outVec.clear();
    for (rapidjson::SizeType i = 0; i < arr.Size(); i++)
    {
        if (true == arr[i].IsString())
        {
            outVec.push_back(arr[i].GetString());
        }
    }

    return true;
}

const rapidjson::Document& RapidJsonWrapper::GetDocument() const
{
    return mDocument;
}

bool RapidJsonWrapper::_ReadFileToString(const std::string& filePath, std::string& out) const
{
    std::ifstream ifs{ filePath };
    if (false == ifs.is_open()) 
    {
        GConsoleLogger->WriteStdOut(Color::RED,
            L"[JsonWrapper] 파일 열기 실패: %s" ,filePath);

        return false;
    }
    std::ostringstream ss;
    ss << ifs.rdbuf();
    out = ss.str();

    return true;
}