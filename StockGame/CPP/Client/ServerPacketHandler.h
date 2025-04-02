#pragma once
#include <functional>
#include "PacketSession.h"
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
bool OnSCEchoAck(PacketSessionRef& session, SCEchoAck& pkt);
bool OnSCEnterGameAck(PacketSessionRef& session, SCEnterGameAck& pkt);
bool OnSCEnterGameNoti(PacketSessionRef& session, SCEnterGameNoti& pkt);
bool OnSCMoveNoti(PacketSessionRef& session, SCMoveNoti& pkt);
bool OnSCStopNoti(PacketSessionRef& session, SCStopNoti& pkt);

class ServerPacketHandler
{
public:
	static void Initialize()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
		{
			GPacketHandler[i] = Handle_INVALID;
		}
		GPacketHandler[SC_EchoAck] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<SCEchoAck>(OnSCEchoAck, session, buffer, len); };
		GPacketHandler[SC_EnterGameAck] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<SCEnterGameAck>(OnSCEnterGameAck, session, buffer, len); };
		GPacketHandler[SC_EnterGameNoti] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<SCEnterGameNoti>(OnSCEnterGameNoti, session, buffer, len); };
		GPacketHandler[SC_MoveNoti] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<SCMoveNoti>(OnSCMoveNoti, session, buffer, len); };
		GPacketHandler[SC_StopNoti] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<SCStopNoti>(OnSCStopNoti, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(CSEchoReq& pkt) { return MakeSendBuffer(pkt, CS_EchoReq); }
	static SendBufferRef MakeSendBuffer(CSEnterGameReq& pkt) { return MakeSendBuffer(pkt, CS_EnterGameReq); }
	static SendBufferRef MakeSendBuffer(CSMoveReq& pkt) { return MakeSendBuffer(pkt, CS_MoveReq); }
	static SendBufferRef MakeSendBuffer(CSStopReq& pkt) { return MakeSendBuffer(pkt, CS_StopReq); }

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
