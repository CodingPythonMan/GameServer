#include "MapData.h"
#include "CoreGlobal.h"
#include "ConsoleLog.h"

bool MapData::LoadFromFile(const std::string& filePath)
{
    // 1) JSON 파일 파싱
    if (false == mJson.ParseFile(filePath))
    {
        GConsoleLogger->WriteStdOut(Color::RED, L"[MapData] \"%S\" 파일 파싱 실패", filePath.c_str());
        return false;
    }

    const rapidjson::Document& doc = mJson.GetDocument();
    if (false == doc.IsObject()) {
        GConsoleLogger->WriteStdOut(Color::RED, L"[MapData] 최상위 JSON이 객체가 아닙니다.");
        return false;
    }

    if (false == doc.HasMember("map_info") || false == doc["map_info"].IsObject()) 
    {
        GConsoleLogger->WriteStdOut(Color::RED, L"[MapData] \"map_info\" 객체가 없습니다.");
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
        GConsoleLogger->WriteStdOut(Color::YELLOW, L"[MapData] \"cell_size\" 정보가 없거나 잘못된 타입입니다.");
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
            L"[MapData] \"size\" 정보가 없거나 잘못된 타입입니다.");
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
                // 배열이 아닌 항목이 섞여 있으면 건너뜀
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
        GConsoleLogger->WriteStdOut(Color::YELLOW, L"[MapData] \"tile_data\" 정보가 없거나 배열이 아닙니다.");
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