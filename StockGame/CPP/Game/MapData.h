#pragma once
#include <string>
#include <vector>
#include "RapidJsonWrapper.h"

class MapData
{
public:
	MapData() = default;
	~MapData() = default;

	bool LoadFromFile(const std::string& filePath);

private:
	int						mCellSizeX;
	int						mCellSizeY;
	int						mMapWidth;
	int						mMapHeight;

	std::vector<std::vector<int>>	mTileData;

	RapidJsonWrapper		mJson;
};