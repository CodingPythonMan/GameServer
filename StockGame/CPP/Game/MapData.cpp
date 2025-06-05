#include "MapData.h"
#include "CoreGlobal.h"
#include "ConsoleLog.h"

bool MapData::LoadFromFile(const std::string& filePath)
{
    // 1) JSON ���� �Ľ�
    if (false == mJson.ParseFile(filePath))
    {
        GConsoleLogger->WriteStdOut(Color::RED, L"[MapData] \"%S\" ���� �Ľ� ����", filePath.c_str());
        return false;
    }

    const rapidjson::Document& doc = mJson.GetDocument();
    if (false == doc.IsObject()) {
        GConsoleLogger->WriteStdOut(Color::RED, L"[MapData] �ֻ��� JSON�� ��ü�� �ƴմϴ�.");
        return false;
    }

    if (false == doc.HasMember("map_info") || false == doc["map_info"].IsObject()) 
    {
        GConsoleLogger->WriteStdOut(Color::RED, L"[MapData] \"map_info\" ��ü�� �����ϴ�.");
        return false;
    }
    const rapidjson::Value& mapInfo = doc["map_info"];

    if (true == mapInfo.HasMember("cell_size") && true == mapInfo["cell_size"].IsObject()) 
    {
        const rapidjson::Value& cellSize = mapInfo["cell_size"];
        if (true == cellSize.HasMember("x") && true == cellSize["x"].IsInt()) 
        {
            mCellSizeX = cellSize["x"].GetInt();
        }
        if (true == cellSize.HasMember("y") && true == cellSize["y"].IsInt()) 
        {
            mCellSizeY = cellSize["y"].GetInt();
        }
    }
    else 
    {
        GConsoleLogger->WriteStdOut(Color::YELLOW, L"[MapData] \"cell_size\" ������ ���ų� �߸��� Ÿ���Դϴ�.");
    }

    if (mapInfo.HasMember("size") && mapInfo["size"].IsObject()) {
        const rapidjson::Value& size = mapInfo["size"];
        if (true == size.HasMember("width") && true == size["width"].IsInt())
        {
            mMapWidth = size["width"].GetInt();
        }
        if (true == size.HasMember("height") && true == size["height"].IsInt()) 
        {
            mMapHeight = size["height"].GetInt();
        }
    }
    else 
    {
        GConsoleLogger->WriteStdOut(
            Color::YELLOW,
            L"[MapData] \"size\" ������ ���ų� �߸��� Ÿ���Դϴ�.");
    }

    mTileData.clear();
    if (doc.HasMember("tile_data") && doc["tile_data"].IsArray()) 
    {
        const rapidjson::Value& td = doc["tile_data"];
        for (rapidjson::SizeType i = 0; i < td.Size(); ++i) 
        {
            const rapidjson::Value& row = td[i];
            if (!row.IsArray()) 
            {
                // �迭�� �ƴ� �׸��� ���� ������ �ǳʶ�
                continue;
            }
            std::vector<int> rowVec;
            for (rapidjson::SizeType j = 0; j < row.Size(); ++j) 
            {
                if (row[j].IsInt()) {
                    rowVec.push_back(row[j].GetInt());
                }
            }
            mTileData.push_back(std::move(rowVec));
        }
    }
    else 
    {
        GConsoleLogger->WriteStdOut(Color::YELLOW, L"[MapData] \"tile_data\" ������ ���ų� �迭�� �ƴմϴ�.");
    }

    GConsoleLogger->WriteStdOut(Color::GREEN,
        L"%S\n"
        L"    Cell size         = (%d, %d)\n"
        L"    size              = (%d x %d)\n"
        L"    tile Row Count    = %d\n",
        filePath.c_str(),
        mCellSizeX, mCellSizeY,
        mMapWidth, mMapHeight,
        static_cast<int>(mTileData.size())
    );

    return true;
}