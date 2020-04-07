#pragma once
#include "ThreadClass.h"

#include "SelectClass.h"

#include "Lobby.h"

/*
// new (new char)로 생성된 메모리 p를 제거 합니다.
#define SAFE_DELETE( p ) { if( p ) { delete ( p ); ( p ) = NULL; } }

// new (new char *) 로 생성된 배열 p를 제거 합니다.
#define SAFE_DELETE_ARRAY( p ) { if( p ) { delete[] ( p ); ( p ) = NULL; } }
*/

//=====================================================

//Channel

//내부에서 SelectClass를 생성함

//=====================================================

class Channel : public ThreadClass<Channel>
{
private:
	WSADATA				wsaData;
	SOCKET				listenSocket;
	SOCKADDR_IN			servAddr;

	SelectClass<Lobby>	selectClass;
	bool				isFull;
	bool				isEmpty;

	Lobby				lobby;

public:
	Channel();
	~Channel();

	bool Init(int _portNum);

	void LoopRun();

	bool GetIsFull() { return isFull; }
	bool GetIsEmpty() { return isEmpty; }

	SOCKET GetListenSocket() { return listenSocket; }

	SelectClass<Lobby>* GetSelectClass() { return &selectClass; }
};

