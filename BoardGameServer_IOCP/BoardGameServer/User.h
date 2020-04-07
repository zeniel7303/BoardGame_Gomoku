#pragma once
#include "Session.h"
//=====================================================

//User

//Session 상속받아 App단에서 쓸 클래스

//=====================================================

class User : public Session
{
private:


public:
	User();
	~User();

	void Reset();

	void SetName(UserData* _data);
	void SetReady(IsReady* _data);

	void OnConnect(SOCKET _socket, int _num, int _portNum);
	void Disconnect();
};

