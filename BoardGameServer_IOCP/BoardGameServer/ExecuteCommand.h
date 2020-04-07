#pragma once
#include "User.h"
#include "packet.h"

#include "SessionManager.h"
#include "Lobby.h"

//=====================================================

//ExecuteCommand

//파싱한 패킷을 해당하는 명령으로 실행하기 위한 클래스
//Update는 Channel에서 Thread로 돌며 계속 모든 세션을 체크해고있다.

//그 아래의 함수들은 전부 패킷관련 함수들

//=====================================================

class ExecuteCommand
{
private:
	SessionManager<User>*	m_sessionManager;
	Lobby*					m_lobby;

public:
	ExecuteCommand();
	~ExecuteCommand();

	void Init(Lobby* _lobby, SessionManager<User>* _sessionManager);

	void Update();

	void Test(User* _user, char* _packet);
	void RegisterUser(User* _user, char* _packet);
	void SendChat(User* _user, WORD _num, char* _packet);
	void CreatingRoom(User* _user, char* _packet);
	void ExittingRoom(User* _user);
	void JoiningRoom(User* _user, char* _packet);
	void ReadyCheck(User* _user, char* _packet);
	void ReqLobbyUserList(User* _user);
	void ReqRoomList(User* _user, char* _packet);
	void GomokuGridPacket(User* _user, char* _packet);
	void GomokuResult(User* _user, char* _packet);

	void SendRoomUserList(int _tempNum, User* _user, bool _isExitted);
};