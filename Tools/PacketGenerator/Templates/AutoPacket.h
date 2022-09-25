#pragma once
#include "Protocol.pb.h"
#include "PacketSession.h"

/*---------------------------------------------------------------------------------------------
								SERVER AUTO PACKET 
----------------------------------------------------------------------------------------------*/

using PacketFunc = std::function<bool(shared_ptr<PacketSession>&, BYTE*, int32)>;
extern PacketFunc GPacketFuncArray[UINT16_MAX];

/*---------------------------------------------------------------------------------------------
									ENUM PROTOCOL
----------------------------------------------------------------------------------------------*/
enum : uint16
{
{%- for pkt in parser.total_pkt %}
	PACKET_{{pkt.name}} = {{pkt.id}},
{%- endfor %}
};

/*---------------------------------------------------------------------------------------------
							 UPDATE PACKET CREATE FUNCTION
----------------------------------------------------------------------------------------------*/
bool PACKET_INVALID(shared_ptr<PacketSession>& session, BYTE* byte, int32 len);
{%- for pkt in parser.recv_pkt %}
bool {{pkt.name}}_FUNC(shared_ptr<PacketSession>& session, Protocol::{{pkt.name}}& pkt);
{%- endfor %}

/*---------------------------------------------------------------------------------------------
								 PACKET MANAGER CLASS
----------------------------------------------------------------------------------------------*/
class {{output}}
{
public:
	/*---------------------------------------------------------------------------------------------
							UPDATE PACKET REGISTER FUNCTION
	----------------------------------------------------------------------------------------------*/
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketFuncArray[i] = PACKET_INVALID;

{%- for pkt in parser.recv_pkt%}
		GPacketFuncArray[PACKET_{{pkt.name}}] = [](std::shared_ptr<PacketSession>& session, BYTE* buffer, int32 len) { return PacketUpdate<Protocol::{{pkt.name}}>({{pkt.name}}_FUNC, session, buffer, len); };
{%- endfor%}
	}

	/*---------------------------------------------------------------------------------------------
							   PUBLIC UPDATE PACKET FUNCTION
	----------------------------------------------------------------------------------------------*/
	static bool PacketUpdate(shared_ptr<PacketSession>& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketFuncArray[header->_ProtocoliD](session, buffer, len);
	}

	/*---------------------------------------------------------------------------------------------
						   PUBLIC MAKE SENDBUFFER REGISTER
	----------------------------------------------------------------------------------------------*/
{%- for pkt in parser.send_pkt%}
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::{{pkt.name}}& pkt) { return MakeSendBuffer(pkt, PACKET_{{pkt.name}}); }
{%- endfor %}

private:
	/*---------------------------------------------------------------------------------------------
					   PRIVATE UPDATE PACKET FUNCTION
	----------------------------------------------------------------------------------------------*/
	template<typename PacketType, typename ProcessFunc>
	static bool PacketUpdate(ProcessFunc func, shared_ptr<PacketSession>& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	/*---------------------------------------------------------------------------------------------
					   PRIVATE MAKE SENDBUFFER REGISTER
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

