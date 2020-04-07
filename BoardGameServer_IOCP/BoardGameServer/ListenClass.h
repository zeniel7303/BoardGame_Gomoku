#pragma once
#include "ThreadClass.h"

#include "User.h"
#include "SessionManager.h"

#include "Lobby.h"

class ListenClass : public ThreadClass<ListenClass>
{
private:
	WSADATA			m_wsaData;
	SOCKET			m_listenSocket;
	SOCKADDR_IN		m_servAddr;

	int				m_portNum;

	SessionManager<User>*	m_sessionManager;

public:
	ListenClass()
	{

	}

	~ListenClass()
	{

	}

	bool Init(SessionManager<User>*	_sessionManager,  int _portNum)
	{
		m_sessionManager = _sessionManager;
		m_portNum = _portNum;

		if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
		{
			printf("Failed WSAStartup() \n");
		}

		m_listenSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (m_listenSocket == INVALID_SOCKET)
		{
			WSACleanup();
			printf("Failed socket() \n");
			return false;
		}

		memset(&m_servAddr, 0, sizeof(m_servAddr));
		m_servAddr.sin_family = AF_INET;
		m_servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		m_servAddr.sin_port = htons(m_portNum);

		//bind 앞에 :: 붙인 이유
		//https://stackoverflow.com/questions/44861571/operator-error
		if (::bind(m_listenSocket, (SOCKADDR*)&m_servAddr, sizeof(m_servAddr)) == SOCKET_ERROR)
		{
			printf("Binding Error \n");
			closesocket(m_listenSocket);
			WSACleanup();
			return false;
		}

		if (listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR)
		{
			printf("Listening Error \n");
			closesocket(m_listenSocket);
			WSACleanup();
			return false;
		}

		printf("Channel Start \n");

		ThreadClass<ListenClass>::Start(this);

		return true;
	}

	void LoopRun()
	{
		SOCKET			sockClient;
		SOCKADDR_IN		addrClient;
		int				addrClientSize = sizeof(addrClient);

		while (1)
		{
			//Accept대기
			sockClient = accept(m_listenSocket, (SOCKADDR*)&addrClient, &addrClientSize);

			//sockClient가 잘못 들어오면 Failed 표시
			if (sockClient == INVALID_SOCKET)
			{
				printf("Failed accept() \n");
			}

			//User 하나를 SessionManager에서 가져와
			User* tempUser = m_sessionManager->CreateSession();
			//SessionManager의 vector에 추가시켜준다.
			m_sessionManager->AddSessionList(tempUser);

			tempUser->OnConnect(sockClient, m_sessionManager->GetCount(), m_portNum);
		}
	}
};

