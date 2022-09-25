#include "pch.h"
#include "IocpManager.h"
#include "PacketSession.h"
#include "ServiceManager.h"
#include "ThreadManager.h"
#include "ClientPacketManager.h"

class CientSession : public PacketSession
{
public:
	CientSession() {};
	~CientSession() {};

	void ContentsConnect()
	{
	}

	void ContentsDisConnect()
	{
	}

	void ContentsSend()
	{
	}

	void ContentsRecv(BYTE* buffer, int32 datasize)
	{
		shared_ptr<PacketSession> session = GetPacketSession();
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

		ClientPacketManager::PacketUpdate(session, buffer, datasize);
	}
};

int main()
{
	ClientPacketManager::Init();

	shared_ptr<ClientManager> client = GMakeShared<ClientManager>(
		NetAddress(L"127.0.0.1", 7777),
		GMakeShared<IocpManager>(),
		GMakeShared<CientSession>,
		5);

	CRASH_IF(client->Start() == false);


	for (int i = 0; i < 1; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					client->GetIocpManager()->IocpUpdate();
				}
			});
	}

	
	GThreadManager->Join();
}

