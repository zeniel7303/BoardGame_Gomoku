#include "ExecuteCommand.h"

ExecuteCommand::ExecuteCommand()
{
}

ExecuteCommand::~ExecuteCommand()
{
}

void ExecuteCommand::Init(Lobby* _lobby, SessionManager<User>* _sessionManager)
{
	m_lobby = _lobby;
	m_sessionManager = _sessionManager;
}

void ExecuteCommand::Update()
{
	vector<User*> vSessionList = m_sessionManager->GetSessionList();

	for (int i = 0; i < vSessionList.size(); i++)
	{
		//연결이 안돼있다.
		if (!vSessionList[i]->GetIsConnected())
		{
			//방에 들어가있었다면 나간다.
			if (vSessionList[i]->GetSessionInfo().roomNum > 0)
			{
				ExittingRoom(vSessionList[i]);
			}

			//로비 유저 리스트에서 제거
			m_lobby->EraseLobbyUser(vSessionList[i]);

			//접속이 끊긴 처리
			closesocket(vSessionList[i]->GetSessionInfo().socket);
			printf("[ close socket : %d ] \n", vSessionList[i]->GetSessionInfo().socket);

			vSessionList[i]->Disconnect();
			m_sessionManager->DeleteSession(vSessionList[i]);

			printf("\n");

			continue;
		}

		//파싱 가능한지 여부 체크
		if (!vSessionList[i]->GetRecvBuffer()->CanParsing()) continue;

		PacketHeader* packet = (PacketHeader*)vSessionList[i]->GetRecvBuffer()->Parsing();

		//패킷에 따른 명령 처리
		switch (packet->cmd)
		{
		case 2000:
			Test(vSessionList[i], (char*)packet);
			break;
		case 0:
			RegisterUser(vSessionList[i], (char*)packet);
			break;
			//LobbyChatting
		case 1:
			SendChat(vSessionList[i], packet->cmd, (char*)packet);
			break;
			//RoomChatting
		case 2:
			SendChat(vSessionList[i], packet->cmd, (char*)packet);
			break;
		case 3:
			CreatingRoom(vSessionList[i], (char*)packet);
			break;
		case 4:
			ExittingRoom(vSessionList[i]);
			break;
		case 5:
			JoiningRoom(vSessionList[i], (char*)packet);
			break;
		case 6:
			//190930
			//c++ c# bool 크기 차이 마샬링
			//https://jacking.tistory.com/1374
			ReadyCheck(vSessionList[i], (char*)packet);
			break;
			//로비 유저리스트 요청
		case 7:
			ReqLobbyUserList(vSessionList[i]);
			break;
			//방 리스트 요청
		case 8:
			ReqRoomList(vSessionList[i], (char*)packet);
			break;
		case 100:
			GomokuGridPacket(vSessionList[i], (char*)packet);
			break;
		case 101:
			GomokuResult(vSessionList[i], (char*)packet);
		}
	}
}
																	   
void ExecuteCommand::Test(User* _user, char* _packet)	   
{
	TestPacket* data = (TestPacket*)_packet;

	//printf("[ socket : %d / %d / %c ]\n", data->socket, data->size, data->test);
	//printf("[ socket : %d ]\t", data->socket);
}

void ExecuteCommand::RegisterUser(User* _user, char* _packet)	   
{
	UserData* data = (UserData*)_packet;

	_user->SetName(data);

	data->cmd = 89;
	data->size = sizeof(UserData);

	printf("[userID : %d / name : %s / socket : %d]\n",
		_user->GetSessionInfo().userID, _user->GetSessionInfo().name, _user->GetSessionInfo().socket);

	m_lobby->PushBackLobbyUser(_user);

	//_user->GetSendBuffer()->Write(sizeof(UserData));
	_user->Send(reinterpret_cast<char*>(data), sizeof(UserData));
}

void ExecuteCommand::SendChat(User* _user, WORD _num, char* _packet)
{
	SendChatting* data = (SendChatting*)_packet;

	printf("[Chatting Message : %s]\n", data->message);

	data->cmd = 91;
	data->nameSize = _user->GetSessionInfo().nameSize;
	strcpy(data->name, _user->GetSessionInfo().name);
	data->size = sizeof(SendChatting);

	//Lobby
	if (_num == 1)
	{
		m_lobby->SendChatAll_Lobby(data);
	}
	//Room
	else if (_num == 2)
	{
		m_lobby->getRoomManager()->SendChattAll_Room(_user->GetSessionInfo().roomNum, data);
	}
}

void ExecuteCommand::CreatingRoom(User* _user, char* _packet)		   
{
	CreateRoom* data = (CreateRoom*)_packet;

	if (m_lobby->getRoomManager()->CreateRoom(_user->GetSendBuffer(), _user))
	{
		m_lobby->EraseLobbyUser(_user);

		SendRoomUserList(0, _user, false);
	}
}

void ExecuteCommand::ExittingRoom(User* _user)				   
{
	int tempRoomNum = _user->GetSessionInfo().roomNum;

	if (m_lobby->getRoomManager()->ExitRoom(_user->GetSendBuffer(), _user))
	{
		m_lobby->PushBackLobbyUser(_user);

		SendRoomUserList(tempRoomNum, _user, true);
	}

	printf("[ %d, %s, %d, %d ]\n", 
		_user->GetSessionInfo().userID, 
		_user->GetSessionInfo().name, 
		_user->GetSessionInfo().socket, 
		_user->GetSessionInfo().roomNum);
}

void ExecuteCommand::JoiningRoom(User* _user, char* _packet)		   
{
	JoinRoom* data = (JoinRoom*)_packet;

	if (m_lobby->getRoomManager()->JoinRoom(_user->GetSendBuffer(), _user, data->roomNum))
	{
		m_lobby->EraseLobbyUser(_user);

		SendRoomUserList(0, _user, false);
	}

	printf("[ %d, %s, %d, %d ]\n", 
		_user->GetSessionInfo().userID, 
		_user->GetSessionInfo().name, 
		_user->GetSessionInfo().socket, 
		_user->GetSessionInfo().roomNum);
}

void ExecuteCommand::ReadyCheck(User* _user, char* _packet)		   
{
	IsReady* data = (IsReady*)_packet;

	_user->SetIsReady(data->isReady);

	data->cmd = 97;
	data->size = sizeof(IsReady);

	//_user->GetSendBuffer()->Write(sizeof(IsReady));
	_user->Send(reinterpret_cast<char*>(data), sizeof(IsReady));

	m_lobby->getRoomManager()->RoomUserUpdate(_user);

	SendRoomUserList(0, _user, false);

	printf("[ %d, %s, %d, %d ]\n", 
		_user->GetSessionInfo().userID, 
		_user->GetSessionInfo().name, 
		_user->GetSessionInfo().socket, 
		_user->GetSessionInfo().roomNum);
}

void ExecuteCommand::ReqLobbyUserList(User* _user)					   
{
	m_lobby->SendLobbyUserList(_user->GetSendBuffer(), _user);
}

void ExecuteCommand::ReqRoomList(User* _user, char* _packet)		   
{
	ReqRoomListData* data = (ReqRoomListData*)_packet;

	m_lobby->SendRoomList(_user->GetSendBuffer(), _user, data);
}

void ExecuteCommand::GomokuGridPacket(User* _user, char* _packet)	   
{
	m_lobby->getRoomManager()->GomokuGridPacket(_user->GetSendBuffer(), &_user->GetSessionInfo(), _packet);
}

void ExecuteCommand::GomokuResult(User* _user, char* _packet)		   
{
	m_lobby->getRoomManager()->GomokuResultPacket(_user->GetSendBuffer(), &_user->GetSessionInfo(), _packet);
}

void ExecuteCommand::SendRoomUserList(int _tempNum, User* _user, bool _isExitted)
{
	m_lobby->getRoomManager()->SendRoomUserList(_user->GetSendBuffer(), _user);

	if (_isExitted)
	{
		m_lobby->getRoomManager()->SendRoomUserListToOther(_user->GetSendBuffer(), _tempNum);
	}
	else
	{
		m_lobby->getRoomManager()->SendRoomUserListToOther(_user->GetSendBuffer(), _user);
	}
}