#include "pch.h"
#include "ClientPacketManager.h"
#include "Protocol.pb.h"

PacketFunc GPacketFuncArray[UINT16_MAX];

bool PACKET_INVALID(shared_ptr<PacketSession>& session, BYTE* byte, int32 len)
{
	return true;
}

bool SERVER_LOGIN_FUNC(shared_ptr<PacketSession>& session, Protocol::SERVER_LOGIN& pkt)
{
	return true;
}

bool SERVER_EXIT_FUNC(shared_ptr<PacketSession>& session, Protocol::SERVER_EXIT& pkt)
{
	return true;
}
