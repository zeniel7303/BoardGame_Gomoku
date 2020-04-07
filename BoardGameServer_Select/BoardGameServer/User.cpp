#include "User.h"

User::User()
{
	
}

User::~User()
{
	//Reset();

	ringBuffer->Reset();

	savedBuffer = nullptr;
	savedPosition = 0;

	tempRoomNum = 0;

	info = {};
}

void User::Init(Lobby* _lobby)
{
	lobby = _lobby;

	tempRoomNum = 0;
}

void User::Reset()
{
	ringBuffer->Reset();

	savedBuffer = nullptr;
	savedPosition = 0;

	tempRoomNum = 0;

	info = {};
}

bool User::Recv()
{
	recvByte = recv(info.socket, ringBuffer->GetWritePoint(), ringBuffer->GetWriteSize(), 0);
	printf("writeable size : %d \n", ringBuffer->GetWriteSize());
	printf("recvByte : %d \n", recvByte);
	ringBuffer->Write(recvByte);
	printf("data in buffer = %d \n", ringBuffer->GetDataInBuffer());

	//연결 해제
	if (recvByte <= 0)
	{
		//int errorCode = WSAGetLastError();

		if (info.roomNum > 0)
		{
			ExittingRoom();
		}

		lobby->LogOut(&info);

		//접속이 끊긴 처리
		closesocket(info.socket);
		printf("close socket : %d \n", info.socket);

		printf("\n");
		return true;
	}

	bool tempBool = false;

	tempBool = Parsing();

	//버퍼에 데이터가 더 남아있는 경우 한번 더 체크해본다.
	if (ringBuffer->GetDataInBuffer() > 2)
	{
		tempBool = Parsing();
	}

	return tempBool;
}

bool User::Parsing()
{
	char* tempBuffer = nullptr;
	unsigned short  packetSize = 0;
	PacketHeader* packetHeaderData = nullptr;

	if (ringBuffer->GetDataInBuffer() < 2)
	{
		return false;
	}

	tempBuffer = ringBuffer->GetReadPoint();
	packetSize = *(unsigned short *)tempBuffer;
	printf("packet Size = %d \n", packetSize);

	if (ringBuffer->GetDataInBuffer() < packetSize)
	{
		return false;
	}

	if (ringBuffer->GetReadPoint() > ringBuffer->GetWritePoint())
	{
		savedBuffer = ringBuffer->GetReadPoint();
		savedPosition = ringBuffer->GetRemainedSize(ringBuffer->GetReadPoint());
		ringBuffer->Read(ringBuffer->GetRemainedSize(ringBuffer->GetReadPoint()));

		memcpy(savedBuffer + savedPosition, ringBuffer->GetReadPoint(),
			packetSize - savedPosition);

		ringBuffer->Read(packetSize - savedPosition);

		tempBuffer = savedBuffer;
		packetHeaderData = (PacketHeader*)tempBuffer;

		savedBuffer = nullptr;
		savedPosition = 0;
	}
	else
	{
		packetHeaderData = (PacketHeader*)tempBuffer;
		ringBuffer->Read(packetSize);
	}

	printf("[ cmd : %d ]\n", packetHeaderData->cmd);

	//헤더의 커맨드가 뭐인지에 따라 작동할 것 선택z
	switch (packetHeaderData->cmd)
	{
	case 0:
		RegisterUser(tempBuffer);
		break;
		//LobbyChatting
	case 1:
		SendChat(packetHeaderData->cmd, tempBuffer);
		break;
		//RoomChatting
	case 2:
		SendChat(packetHeaderData->cmd, tempBuffer);
		break;
	case 3:
		CreatingRoom(tempBuffer);
		break;
	case 4:
		ExittingRoom();
		break;
	case 5:
		JoiningRoom(tempBuffer);
		break;
	case 6:
		//190930
		//c++ c# bool 크기 차이 마샬링
		//https://jacking.tistory.com/1374
		ReadyCheck(tempBuffer);
		break;
		//로비 유저리스트 요청
	case 7:
		ReqLobbyUserList();
		break;
		//방 리스트 요청
	case 8:
		ReqRoomList(tempBuffer);
		break;
	case 100:
		GomokuGridPacket(tempBuffer);
		break;
	case 101:
		GomokuResult(tempBuffer);
	}

	printf("data in buffer = %d \n", ringBuffer->GetDataInBuffer());

	//버퍼에 데이터가 더 남아있는 경우 한번 더 체크해본다.
	if (ringBuffer->GetDataInBuffer() > 2)
	{
		Parsing();
	}

	return false;
}

void User::Send(void* _data, int _size)
{
	send(info.socket, (char*)_data, _size, 0);
}

void User::RegisterUser(SOCKET _socket, int _num, int _portNum)
{
	UserData* data = new UserData();
	data->cmd = 90;
	data->userID = _num;
	data->socket = _socket;
	data->roomNum = 0;
	data->size = sizeof(UserData);

	info.userID = _num;
	info.socket = _socket;
	info.roomNum = 0;

	send(_socket, (char*)data, sizeof(UserData), 0);

	printf("[client socket : %d / count : %d / portNum : %d]\n", _socket, _num, _portNum);
	printf("================================================== \n");
}

void User::RegisterUser(char* _packet)
{
	UserData* data = (UserData*)_packet;

	strcpy(info.name, data->name);
	info.nameSize = strlen(info.name);

	data->cmd = 89;
	data->size = sizeof(UserData);

	printf("[userID : %d / name : %s / socket : %d]\n", info.userID, info.name, info.socket);

	lobby->pushBackLobbyUser(&info);

	send(info.socket, (char*)data, sizeof(UserData), 0);
}

void User::SendChat(WORD _num, char* _packet)
{
	SendChatting* data = (SendChatting*)_packet;

	printf("[Chatting Message : %s]\n", data->message);

	data->cmd = 91;
	data->nameSize = info.nameSize;
	strcpy(data->name, info.name);
	data->size = sizeof(SendChatting);

	//Lobby
	if (_num == 1)
	{
		lobby->SendChatAll_Lobby(data);
	}
	//Room
	else if (_num == 2)
	{
		lobby->getRoomManager()->SendChattAll_Room(info.roomNum, data);
	}
}

void User::CreatingRoom(char* _packet)
{
	CreateRoom* data = (CreateRoom*)_packet;

	if (lobby->getRoomManager()->CreateRoom(&info))
	{
		lobby->LobbyUserUpdata(&info, true);

		SendRoomUserList(false);
	}

	printf("[ id : %d, name : %s, socket : %d ]\n", info.userID, info.name, info.socket);
}

void User::ExittingRoom()
{
	tempRoomNum = info.roomNum;

	if (lobby->getRoomManager()->ExitRoom(&info))
	{
		lobby->LobbyUserUpdata(&info, false);

		SendRoomUserList(true);
	}

	printf("[ %d, %s, %d, %d ]\n", info.userID, info.name, info.socket, info.roomNum);

}

void User::JoiningRoom(char* _packet)
{
	JoinRoom* data = (JoinRoom*)_packet;

	if (lobby->getRoomManager()->JoinRoom(&info, data->roomNum))
	{
		lobby->LobbyUserUpdata(&info, true);

		SendRoomUserList(false);
	}

	printf("[ %d, %s, %d, %d ]\n", info.userID, info.name, info.socket, info);

}

void User::ReadyCheck(char* _packet)
{
	IsReady* data = (IsReady*)_packet;

	info.isReady = data->isReady;

	data->cmd = 97;
	data->size = sizeof(IsReady);

	send(info.socket, (char*)data, sizeof(IsReady), 0);

	lobby->getRoomManager()->RoomUserUpdate(&info);

	SendRoomUserList(false);

	printf("[ %d, %s, %d, %d ]\n", info.userID, info.name, info.socket, info.roomNum);
}

void User::ReqLobbyUserList()
{
	lobby->SendLobbyUserList(&info);
}

void User::ReqRoomList(char* _packet)
{
	ReqRoomListData* data = (ReqRoomListData*)_packet;

	lobby->SendRoomList(&info, data);
}

void User::GomokuGridPacket(char* _packet)
{
	lobby->getRoomManager()->GomokuGridPacket(&info, _packet);
}

void User::GomokuResult(char* _packet)
{
	lobby->getRoomManager()->GomokuResultPacket(&info, _packet);
}

void User::LobbyUserUpdate(bool _goRoom)
{
	lobby->LobbyUserUpdata(&info, _goRoom);
}

void User::SendRoomUserList(bool _isExitted)
{
	lobby->getRoomManager()->SendRoomUserList_(&info);

	if (_isExitted)
	{
		lobby->getRoomManager()->SendRoomUserListToOther_(tempRoomNum);

		tempRoomNum = 0;
	}
	else
	{
		lobby->getRoomManager()->SendRoomUserListToOther_(&info);
	}
}