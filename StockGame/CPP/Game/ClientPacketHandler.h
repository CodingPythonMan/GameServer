#pragma once
#include <functional>
#include "GameSession.h"
#include "Macro.h"
#include "CoreGlobal.h"
#include "SendBuffer.h"
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	CS_EchoReq = 1,
	SC_EchoAck = 2,
	CS_EnterGameReq = 3,
	SC_EnterGameAck = 4,
	SC_EnterGameNoti = 5,
	CS_MoveReq = 6,
	SC_MoveNoti = 7,
	CS_StopReq = 8,
	SC_StopNoti = 9,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool OnCSEchoReq(PacketSessionRef& session, CSEchoReq& pkt);
bool OnCSEnterGameReq(PacketSessionRef& session, CSEnterGameReq& pkt);
bool OnCSMoveReq(PacketSessionRef& session, CSMoveReq& pkt);
bool OnCSStopReq(PacketSessionRef& session, CSStopReq& pkt);

class ClientPacketHandler
{
public:
	static void Initialize()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
		{
			GPacketHandler[i] = Handle_INVALID;
		}
		GPacketHandler[CS_EchoReq] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<CSEchoReq>(OnCSEchoReq, session, buffer, len); };
		GPacketHandler[CS_EnterGameReq] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<CSEnterGameReq>(OnCSEnterGameReq, session, buffer, len); };
		GPacketHandler[CS_MoveReq] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<CSMoveReq>(OnCSMoveReq, session, buffer, len); };
		GPacketHandler[CS_StopReq] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<CSStopReq>(OnCSStopReq, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(SCEchoAck& pkt) { return MakeSendBuffer(pkt, SC_EchoAck); }
	static SendBufferRef MakeSendBuffer(SCEnterGameAck& pkt) { return MakeSendBuffer(pkt, SC_EnterGameAck); }
	static SendBufferRef MakeSendBuffer(SCEnterGameNoti& pkt) { return MakeSendBuffer(pkt, SC_EnterGameNoti); }
	static SendBufferRef MakeSendBuffer(SCMoveNoti& pkt) { return MakeSendBuffer(pkt, SC_MoveNoti); }
	static SendBufferRef MakeSendBuffer(SCStopNoti& pkt) { return MakeSendBuffer(pkt, SC_StopNoti); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;
		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};
