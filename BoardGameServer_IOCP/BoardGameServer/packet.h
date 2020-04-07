#pragma once
#include <WS2tcpip.h>

#define LOBBYMAX 100

#define maxComplement 2

using namespace std;

#pragma pack(push, 1)

struct PacketHeader
{
	PacketHeader()
	{
		memset(this, 0, sizeof(PacketHeader));
	}

	WORD size;
	WORD cmd;
	int socket;
};

struct userInformation
{
	int socket;

	int userID;
	int roomNum;
	WORD nameSize;

	char name[32];

	bool isReady;
};

struct userInfoIOCP
{
	WSAOVERLAPPED recvOverlapped;
	WSABUF recvDataBuffer;

	WSAOVERLAPPED sendOverlapped;
	WSABUF sendDataBuffer;

	userInformation baseInfo;
};

struct SendingRoomStruct
{
	bool isPlaying = false;
	int index = 0;
	int count = 0;
	int complement = maxComplement;
};

template <typename T>
struct RoomStruct
{
	SendingRoomStruct roomInfo;

	T* user[2];

	void Reset()
	{
		roomInfo.complement = 0;
		roomInfo.count = 0;
		roomInfo.index = 0;
		roomInfo.isPlaying = false;

		user[0] = NULL;
		user[1] = NULL;
	}
};

//È¦¼ö·Î
struct TestPacket : PacketHeader
{
	char test = 'A';
};

struct UserData : PacketHeader
{
	int userID;
	int roomNum;
	WORD nameSize;

	char name[32];
};

struct ParticipantsInfo : PacketHeader
{
	//int page;
	int count = 0;
	int arraySize;

	userInformation BelongInfos[LOBBYMAX];
};

struct CreatedRoomList : PacketHeader
{
	//int page;
	int count = 0;
	int arraySize;

	SendingRoomStruct roomList[12];
};

struct SendChatting : PacketHeader
{
	SendChatting()
	{
		memset(this, 0, sizeof(SendChatting));
	}

	WORD nameSize;
	char name[32];

	char message[255];
};

struct CreateRoom : PacketHeader
{
	CreateRoom()
	{
		memset(this, 0, sizeof(CreateRoom));
	}

	WORD roomNum;
};

struct DestroyRoom : PacketHeader
{
	DestroyRoom()
	{
		memset(this, 0, sizeof(DestroyRoom));
	}

	WORD roomNum;
};

struct JoinRoom : PacketHeader
{
	JoinRoom()
	{
		memset(this, 0, sizeof(JoinRoom));
	}

	WORD roomNum;
};

struct IsReady : PacketHeader
{
	IsReady()
	{
		memset(this, 0, sizeof(IsReady));
	}

	bool isReady;
};

struct ReqRoomListData : PacketHeader
{
	int page;
};

struct GomokuGrid : PacketHeader
{
	GomokuGrid()
	{
		memset(this, 0, sizeof(GomokuGrid));
	}

	int px;
	int py;
	bool isBlack;
};

struct GomokuResult : PacketHeader
{
	GomokuResult()
	{
		memset(this, 0, sizeof(GomokuResult));
	}

	bool isBlack;
};

struct InGameUserInfo
{
	userInformation user;

	int order;
};

#pragma pack(pop)