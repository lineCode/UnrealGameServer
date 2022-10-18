# GameServer
> Since 2022.08.11
## 기본 서버 패치노트
- 2022.08.11      
MemoryPool 추가  
MemoryManager 추가  
ObjectPool 추가  
MemoryPool들 Shared_ptr 추가

- 2022.08.12  
CoreTLS 추가  
ReadWriteLock 추가  
Lock Macro 추가  
Crash Macro 추가  
MemoryPool STL 추가

- 2022.08.13  
GThreadManager 전용 RWLOCK 매크로 추가  
ThreadManager 추가  
NetAddress 추가  
SocketUtils 추가

- 2022.08.14  
IOCP Object 추가  
IOCP Storage 추가  
IOCP Manager 추가

- 2022.08.16  
ListenManager 추가  
Session 추가 

- 2022.08.17  
Session 코드변경 및 기능추가

- 2022.08.18  
RecvBuffer 추가

- 2022.08.19  
SendBuffer 추가  
PacketSession 추가    
모든 Session shared_ptr로 변경  

- 2022.08.20  
ServiceManager 추가  
ServerManager 추가  
ClientManager 추가  
PacketCheckRecv 조립 효율적으로 변경  
Send 함수 버그 수정 및 편리성 업데이트

- 2022.08.23  
BufferReadr 추가  
BufferWriter 추가  
Send 및 Recv 편리성 강화  
SendtoBuffer 함수 추가    
RecvtoBuffer 함수 추가

- 2022.08.24  
ServerPacketManager 추가  
ClientPacketManager 추가

- 2022.08.28  
Protobuf 추가

- 2022.08.29  
Packet 자동화 추가

- 2022.09.01  
JobQueue 추가

- 2022.09.02  
JobTimer 추가 

- 2022.09.05  
GJobQueueManager 접근 문제 수정

- 2022.10.05    
Room RWLOCK 모두 제거하고 Jobqueue 방식으로 변경

- 2022.10.06  
DBConnection , DBConnectionPool 추가

- 2022.10.07  
DBbind 추가

- 2022.10.12  
XMLParser 추가  
JsonParer 추가  
DBModel 추가  
XmlDBSynchronizer(XMLORM) 추가  
JsonDBSynchronizer(JsonORM) 추가  
Xml 과 Json 둘중 Json으로 DB작업 관리

- 2022.10.13  
Json 파일 파싱 후 Procedure 자동화 추가

- 2022.10.16  
ConverString 추가

## 언리얼 서버 패치노트
- 2022.09.11   
InGame 로직들 추가  
->RoomManager , PlayerManager , Room , Player

- 2022.09.12  
플레이어 방 접속 크래쉬 버그 수정  
언리얼과 캐릭터 접속 동기화 추가

- 2022.09.13  
플레이어 이동 동기화 추가

- 2022.09.18  
플레이어 스킬 동기화 추가

- 2022.09.26  
플레이어 스킬 동기화 패킷 리팩토링  
플레이어 스킬 애니메이션 동기화 추가  
ObjectUtils 추가  
GitIgnore 대규모 변경

- 2022.09.28  
캐릭터 이동 동기화 리팩토링 -> CS 방식으로 변경  
스킬 패킷 리팩토링  
Gideon 캐릭터 추가

- 2022.09.29  
몬스터 Sevarog 추가  
AI 기본 상태 머신 함수 추가  
AI 타겟 추적 기능 추가

- 2022.10.02   
RapidJson 추가  
DataManager 추가  
ConfigManager 추가  

- 2022.10.03  
안전성 향상을 위해 Thread -> JThread로 변경  
스텟 연동 추가  
데미지 연동 추가

- 2022.10.04  
Dead 연동 추가

- 2022.10.16  
Player DB 연동 추가

- 2022.10.18  
DBJobManager 추가  
Player Stat DB 연동 추가