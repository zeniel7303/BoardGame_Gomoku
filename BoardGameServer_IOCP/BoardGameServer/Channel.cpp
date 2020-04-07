#include "Channel.h"

Channel::Channel()
{
}

Channel::~Channel()
{
}

bool Channel::Init(int _portNum)
{
	m_lobby.Init();
	//상호참조용 함수들
	m_lobby.SetChannelData(this);

	m_sessionManager.Init();

	//일단 서버의 내부 기능들 부터 만들고 listen과 accpet를 시작하자
	if (!m_listenClass.Init(&m_sessionManager, _portNum)) return false;

	//이후 패킷처리 부분 시작
	m_executeCommand.Init(&m_lobby, &m_sessionManager);

	ThreadClass<Channel>::Start(this);

	return  true;
}

//채널의 쓰레드
void Channel::LoopRun()
{
	while (1)
	{
		m_executeCommand.Update();

		Sleep(1);
	}
}