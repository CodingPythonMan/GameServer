#pragma once
#include "RefSingleton.h"
#include "Macro.h"
#include <vector>
#include <unordered_map>
#include <iostream>

struct MonitorData
{
	int mRecvCount = 0;
	int mSendCount = 0;
};

class Monitoring : public RefSingleton<Monitoring>
{
public:
	void RegisterData(MonitorData* data)
	{
		WRITE_LOCK;
		mDataList.push_back(data);
	}

	void Start()
	{
		while (false == mIsStop)
		{
			int totalRecv = 0;
			int totalSend = 0;

			{
				WRITE_LOCK;
				for (MonitorData* data : mDataList)
				{
					int diffRecv = data->mRecvCount - mPrevSnapshot[data].mRecvCount;
					int diffSend = data->mSendCount - mPrevSnapshot[data].mSendCount;

					totalRecv += diffRecv;
					totalSend += diffSend;

					mPrevSnapshot[data].mRecvCount = data->mRecvCount;
					mPrevSnapshot[data].mSendCount = data->mSendCount;
				}
			}

			std::cout << "[Monitor] Recv: " << totalRecv << ", Send: " << totalSend << std::endl;

			std::this_thread::sleep_for(std::chrono::seconds(10));
		}
	}

	void ResetRecvSnapshot(MonitorData* data, int& recvCount)
	{
		WRITE_LOCK;
		mPrevSnapshot[data].mRecvCount = recvCount;
		recvCount = 0;
	}

	void ResetSendSnapshot(MonitorData* data, int& sendCount)
	{
		WRITE_LOCK;
		mPrevSnapshot[data].mSendCount = sendCount;
		sendCount = 0;
	}
	
private:
	USE_LOCK;
	std::unordered_map<MonitorData*, MonitorData>	mPrevSnapshot;
	std::vector<MonitorData*>						mDataList;
	std::atomic<bool>								mIsStop = false;
};