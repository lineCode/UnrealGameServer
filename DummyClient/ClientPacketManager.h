#pragma once
#include "Protocol.pb.h"
#include "PacketSession.h"

/*---------------------------------------------------------------------------------------------
										AUTO PACKET
-----------------------------------------------------------------------------------------------
Developer : LeeMinGu
Update    : 2022.08.29
----------------------------------------------------------------------------------------------*/

using PacketFunc = std::function<bool(shared_ptr<PacketSession>&, BYTE*, int32)>;
extern PacketFunc GPacketFuncArray[UINT16_MAX];

enum : uint16
{
	PACKET_SERVER_LOGIN = 1000,
	PACKET_CLIENT_LOGIN = 1001,
	PACKET_SERVER_EXIT = 1002,
	PACKET_CLIENT_EXIT = 1003,
};

bool PACKET_INVALID(shared_ptr<PacketSession>& session, BYTE* byte, int32 len);
bool SERVER_LOGIN_FUNC(shared_ptr<PacketSession>& session, Protocol::SERVER_LOGIN& pkt);
bool SERVER_EXIT_FUNC(shared_ptr<PacketSession>& session, Protocol::SERVER_EXIT& pkt);

class ClientPacketManager
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketFuncArray[i] = PACKET_INVALID;
		GPacketFuncArray[PACKET_SERVER_LOGIN] = [](std::shared_ptr<PacketSession>& session, BYTE* buffer, int32 len) { return PacketUpdate<Protocol::SERVER_LOGIN>(SERVER_LOGIN_FUNC, session, buffer, len); };
		GPacketFuncArray[PACKET_SERVER_EXIT] = [](std::shared_ptr<PacketSession>& session, BYTE* buffer, int32 len) { return PacketUpdate<Protocol::SERVER_EXIT>(SERVER_EXIT_FUNC, session, buffer, len); };
	}

	static bool PacketUpdate(shared_ptr<PacketSession>& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketFuncArray[header->_ProtocoliD](session, buffer, len);
	}
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::CLIENT_LOGIN& pkt) { return MakeSendBuffer(pkt, PACKET_CLIENT_LOGIN); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::CLIENT_EXIT& pkt) { return MakeSendBuffer(pkt, PACKET_CLIENT_EXIT); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool PacketUpdate(ProcessFunc func, shared_ptr<PacketSession>& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static shared_ptr<SendBuffer> MakeSendBuffer(T& pkt, uint16 pktid)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 PacketSize = dataSize + sizeof(PacketHeader);

		shared_ptr<SendBuffer> sendbuffer = GSendBufferManager->Open(PacketSize);

		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendbuffer->GetBuffer());
		header->_size = PacketSize;
		header->_ProtocoliD = pktid;

		pkt.SerializeToArray(&header[1], dataSize);
		sendbuffer->Close(PacketSize);

		return sendbuffer;
	}
};
