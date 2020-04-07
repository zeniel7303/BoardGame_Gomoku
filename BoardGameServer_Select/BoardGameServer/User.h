#pragma once
#include "Session.h"
#include "Lobby.h"

//=====================================================

//User

//Session 상속받아 App단에서 쓸 클래스

//=====================================================

class User : public Session<Lobby>
{
private:
	Lobby* lobby;

	int tempRoomNum;

public:
	User();
	~User();

	void Init(Lobby* _lobby);
	void Reset();
	bool Recv();
	bool Parsing();
	void Send(void* _data, int _size);

	void RegisterUser(SOCKET _socket, int _num, int _portNum);
	void RegisterUser(char* _packet);

	void SendChat(WORD _num, char* _packet);
	void CreatingRoom(char* _packet);
	void ExittingRoom();
	void JoiningRoom(char* _packet);
	void ReadyCheck(char* _packet);
	void ReqLobbyUserList();
	void ReqRoomList(char* _packet);
	void GomokuGridPacket(char* _packet);
	void GomokuResult(char* _packet);

	void LobbyUserUpdate(bool _goRoom);
	void SendRoomUserList(bool _isExitted);
};

