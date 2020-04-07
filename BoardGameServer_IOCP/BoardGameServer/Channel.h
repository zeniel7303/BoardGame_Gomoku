#pragma once
#include "ThreadClass.h"

#include "ListenClass.h"

#include "Lobby.h"

#include "User.h"
#include "SessionManager.h"

#include "ExecuteCommand.h"

//=====================================================

//Channel

//Channel 하나당 ListenClass, SessionManager, Lobby를 가지고있음.
//ListenClass에서 Accept받는 중.
//ExecuteCommand 클래스를 이용해 LoopRun에서 해당 채널의 유저들에게 패킷 처리 중

//=====================================================

class Channel : public ThreadClass<Channel>
{
private:
	ListenClass				m_listenClass;

	Lobby					m_lobby;
	SessionManager<User>	m_sessionManager;

	ExecuteCommand			m_executeCommand;

public:
	Channel();
	~Channel();

	bool Init(int _portNum);

	void LoopRun();

	Lobby	GetLobby() { return m_lobby; }
	SessionManager<User> GetSessionManager() { return m_sessionManager; }
};