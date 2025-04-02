#include "ServerPacketHandler.h"

bool OnSCEchoAck(PacketSessionRef& session, SCEchoAck& pkt)
{
	return false;
}

bool OnSCEnterGameAck(PacketSessionRef& session, SCEnterGameAck& pkt)
{
	return false;
}

bool OnSCEnterGameNoti(PacketSessionRef& session, SCEnterGameNoti& pkt)
{
	return false;
}

bool OnSCMoveNoti(PacketSessionRef& session, SCMoveNoti& pkt)
{
	return false;
}

bool OnSCStopNoti(PacketSessionRef& session, SCStopNoti& pkt)
{
	return false;
}