#pragma once
#include "Protocol.pb.h"
#include "ServerSession.h"

/*---------------------------------------------------------------------------------------------
								GAMESERVER AUTO PACKET
-----------------------------------------------------------------------------------------------
Developer : LeeMinGu
Update    : 2022.09.12
----------------------------------------------------------------------------------------------*/

using PacketFunc = std::function<bool(shared_ptr<ServerSession>&, BYTE*, int32)>;
extern PacketFunc GPacketFuncArray[UINT16_MAX];

/*---------------------------------------------------------------------------------------------
									ENUM PROTOCOL
----------------------------------------------------------------------------------------------*/
enum : uint16
{
	PACKET_SERVER_CONNECT = 1000,
	PACKET_SERVER_ENTERGAME = 1001,
	PACKET_SERVER_LEAVEGAME = 1002,
	PACKET_SERVER_SPAWN = 1003,
	PACKET_SERVER_DESTROY = 1004,
	PACKET_SERVER_MOVE = 1005,
	PACKET_SERVER_SKILL = 1006,
	PACKET_SERVER_CHANGEHP = 1007,
	PACKET_SERVER_DIE = 1008,
	PACKET_CLIENT_ENTERGAME = 1009,
	PACKET_CLIENT_MOVE = 1010,
	PACKET_CLIENT_SKILL = 1011,
	PACKET_CLIENT_DAMAGE = 1012,
};

/*---------------------------------------------------------------------------------------------
							  RECV PACKET FUNCTION CREATE
----------------------------------------------------------------------------------------------*/
bool PACKET_INVALID(shared_ptr<ServerSession>& session, BYTE* byte, int32 len);
bool CLIENT_ENTERGAME_FUNC(shared_ptr<ServerSession>& session, Protocol::CLIENT_ENTERGAME& pkt);
bool CLIENT_MOVE_FUNC(shared_ptr<ServerSession>& session, Protocol::CLIENT_MOVE& pkt);
bool CLIENT_SKILL_FUNC(shared_ptr<ServerSession>& session, Protocol::CLIENT_SKILL& pkt);
bool CLIENT_DAMAGE_FUNC(shared_ptr<ServerSession>& session, Protocol::CLIENT_DAMAGE& pkt);

/*---------------------------------------------------------------------------------------------
							 GAMESERVER PACKET MANAGER CLASS
----------------------------------------------------------------------------------------------*/
class ServerPacketManager
{
public:
	/*---------------------------------------------------------------------------------------------
								  RECV PACKET FUNCTION REGISETR
	----------------------------------------------------------------------------------------------*/
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketFuncArray[i] = PACKET_INVALID;
		GPacketFuncArray[PACKET_CLIENT_ENTERGAME] = [](std::shared_ptr<ServerSession>& session, BYTE* buffer, int32 len) { return PacketUpdate<Protocol::CLIENT_ENTERGAME>(CLIENT_ENTERGAME_FUNC, session, buffer, len); };
		GPacketFuncArray[PACKET_CLIENT_MOVE] = [](std::shared_ptr<ServerSession>& session, BYTE* buffer, int32 len) { return PacketUpdate<Protocol::CLIENT_MOVE>(CLIENT_MOVE_FUNC, session, buffer, len); };
		GPacketFuncArray[PACKET_CLIENT_SKILL] = [](std::shared_ptr<ServerSession>& session, BYTE* buffer, int32 len) { return PacketUpdate<Protocol::CLIENT_SKILL>(CLIENT_SKILL_FUNC, session, buffer, len); };
		GPacketFuncArray[PACKET_CLIENT_DAMAGE] = [](std::shared_ptr<ServerSession>& session, BYTE* buffer, int32 len) { return PacketUpdate<Protocol::CLIENT_DAMAGE>(CLIENT_DAMAGE_FUNC, session, buffer, len); };
	}

	/*---------------------------------------------------------------------------------------------
								 PUBLIC PACKET UPDATE FUNCTION
	----------------------------------------------------------------------------------------------*/
	static bool PacketUpdate(shared_ptr<ServerSession>& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketFuncArray[header->_ProtocoliD](session, buffer, len);
	}

	/*---------------------------------------------------------------------------------------------
							 PUBLIC MAKE SENDBUFFER FUNCTION CREATE
	----------------------------------------------------------------------------------------------*/
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::SERVER_CONNECT& pkt) { return MakeSendBuffer(pkt, PACKET_SERVER_CONNECT); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::SERVER_ENTERGAME& pkt) { return MakeSendBuffer(pkt, PACKET_SERVER_ENTERGAME); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::SERVER_LEAVEGAME& pkt) { return MakeSendBuffer(pkt, PACKET_SERVER_LEAVEGAME); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::SERVER_SPAWN& pkt) { return MakeSendBuffer(pkt, PACKET_SERVER_SPAWN); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::SERVER_DESTROY& pkt) { return MakeSendBuffer(pkt, PACKET_SERVER_DESTROY); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::SERVER_MOVE& pkt) { return MakeSendBuffer(pkt, PACKET_SERVER_MOVE); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::SERVER_SKILL& pkt) { return MakeSendBuffer(pkt, PACKET_SERVER_SKILL); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::SERVER_CHANGEHP& pkt) { return MakeSendBuffer(pkt, PACKET_SERVER_CHANGEHP); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::SERVER_DIE& pkt) { return MakeSendBuffer(pkt, PACKET_SERVER_DIE); }

private:
	/*---------------------------------------------------------------------------------------------
								 PRIVATE PACKET UPDATE FUNCTION
	----------------------------------------------------------------------------------------------*/
	template<typename PacketType, typename ProcessFunc>
	static bool PacketUpdate(ProcessFunc func, shared_ptr<ServerSession>& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	/*---------------------------------------------------------------------------------------------
					            PRIVATE MAKE SENDBUFFER FUNCTION
	----------------------------------------------------------------------------------------------*/
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
