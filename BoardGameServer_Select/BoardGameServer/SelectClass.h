#pragma once
#include <winsock2.h>
#include <WS2tcpip.h>
#include <process.h>

#include <map>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

#include "packet.h"

#include "ObjectPool.h"
#include "ThreadClass.h"

#include "User.h"

#define MAX 100

using namespace std;

//=====================================================

//SelectClass

//Server에서 받아온 Lobby 데이터를 Session으로 전송(자세한건 Server.h에 적어둠)

//Session은 Map으로 데이터 저장 -> 이후 비동기 배운 후엔 Map 쓸 이유가 없으니 Select때만 사용하므로 Select에서 Map으로 Session을 관리해두자.

//=====================================================

template<typename T>
class SelectClass : public ThreadClass<SelectClass<T>>, public ObjectPool<User>
{
private:
	T* t;

	//============================================

	//Server.h에서 가져와 저장
	SOCKET			ChannelListenSocket;

	fd_set			fdSocketInfors;		// socket 정보를 저장하고 있는 구조체
	fd_set			fdReads;			// 변화를 읽어올 구조체
	int				iRet;
	SOCKET			sockClient, sockTemp;
	SOCKADDR_IN		addrClient;

	int				portNum;

	//============================================

	//Session은 맵으로 Select에서 저장 (이유는 위에 서술)
	typedef map<SOCKET, User*>					mSessionList;
	typedef map<SOCKET, User*>::iterator		mSessionIter;

	mSessionList				mSession;

	int							count;

	//============================================

	User* tempSession;

	//============================================

	bool isFull;

public:
	SelectClass()
	{

	}

	~SelectClass()
	{

	}

	bool GetIsFull() { return isFull; }
	int GetSessionSize() { return mSession.size(); }

	void Init(SOCKET _listenSocket, int _portNum, T* _t)
	{
		ChannelListenSocket = _listenSocket;
		portNum = _portNum;

		FD_ZERO(&fdSocketInfors);
		FD_SET(ChannelListenSocket, &fdSocketInfors);

		t = _t;

		count = 0;
	}

	bool Blocking()
	{
		fdReads = fdSocketInfors;
		//blocking - 감시할 소켓을 넣고 대기
		iRet = select(0, &fdReads, 0, 0, 0);

		if (iRet == SOCKET_ERROR) return false;
	}

	void ModifyingCheck()
	{
		for (u_int i = 0; i < fdSocketInfors.fd_count; i++)
		{
			sockTemp = fdSocketInfors.fd_array[i];

			//보유한 소켓 중에 변화가 일어난 소켓이 있는지
			if (FD_ISSET(sockTemp, &fdReads))
			{
				//Reads 가 일어난 소켓이 서버인 경우 accept
				if (sockTemp == ChannelListenSocket)
				{
					this->AcceptProcess();
				}
				else //client일 경우 recv 처리
				{
					this->RecvProcess();
				}
			}
		}
	}

	void AcceptProcess()
	{
		//Reads 가 일어난 소켓이 서버인 경우 accept
		int temp = sizeof(addrClient);
		sockClient = accept(ChannelListenSocket, (SOCKADDR*)&addrClient, &temp);

		if (sockClient == INVALID_SOCKET)
		{
			printf("Failed accept() \n");
		}

		this->AddSession(sockClient);

		FD_SET(sockClient, &fdSocketInfors);
	}

	void RecvProcess()
	{
		//client일 경우 recv 처리
		printf("[Recv Start]\n");

		//true -> 접속 종료됨	
		if (this->FindSession(sockTemp)->Recv() == true)
		{
			//임시 저장
			//임시저장을 안해서 한 이유
			//  -> 굳이 Recv할때마다 임시저장안하고 삭제할떄만 ObjectPool에 반납하기 위해 임시저장한다.
			tempSession = this->FindSession(sockTemp);
			tempSession->Reset();

			//맵에서 삭제 후
			//맵 먼저 삭제 한 이유
			//	-> ObjectPool에 먼저 반납하면 map에 있는게 어떻게 될지 몰라서 먼저 했다.
			DeleteSession(sockTemp);

			//ObjectPool에 반납
			ReturnObject(tempSession);
			tempSession = NULL;

			FD_CLR(sockTemp, &fdSocketInfors);
		}

		printf("[Recv End]\n");
		printf("================================================== \n");
	}

	void AddSession(SOCKET _socket)
	{
		//2019/10/17
		//오브젝트 풀로 바꾼 버젼
		tempSession = PopObject();

		if (tempSession != NULL)
		{
			count++;
			tempSession->Init(t);
			tempSession->RegisterUser(_socket, count, portNum);

			mSession.insert(make_pair(_socket, tempSession));
		}

		tempSession = NULL;
	}

	User* FindSession(SOCKET _socket)
	{
		mSessionIter key = mSession.find(_socket);

		if (key != mSession.end())
		{
			return key->second;
		}

		return NULL;
	}

	void DeleteSession(SOCKET _socket)
	{
		mSessionIter key = mSession.find(_socket);

		if (key != mSession.end())
		{
			mSession.erase(_socket);

			count--;
		}
	}

	void IsFullCheck()
	{
		if (mSession.size() >= 100)
		{
			isFull = true;
		}
		else
		{
			isFull = false;
		}
	}

	void LoopRun()
	{
		while (1)
		{
			if (!this->Blocking())
			{
				break;
			}

			ModifyingCheck();

			IsFullCheck();
		}
	}
};