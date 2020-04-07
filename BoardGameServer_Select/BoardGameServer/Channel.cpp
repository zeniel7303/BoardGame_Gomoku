#include "Channel.h"

Channel::Channel()
{
}

Channel::~Channel()
{
}

bool Channel::Init(int _portNum)
{
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("Failed WSAStartup() \n");
	}

	listenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET)
	{
		WSACleanup();
		printf("Failed socket() \n");
		return false;
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(_portNum);

	//bind 앞에 :: 붙인 이유
	//https://stackoverflow.com/questions/44861571/operator-error
	if (::bind(listenSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		printf("Binding Error \n");
		closesocket(listenSocket);
		WSACleanup();
		return false;
	}

	if (listen(listenSocket, 5) == SOCKET_ERROR)
	{
		printf("Listening Error \n");
		closesocket(listenSocket);
		WSACleanup();
		return false;
	}

	printf("Channel Start \n");

	lobby.Init();
	//상호참조용 함수들
	lobby.SetChannelData(this);
	lobby.getRoomManager()->SetChannelData(this);

	selectClass.Init(listenSocket, _portNum, &lobby);

	return true;
}

void Channel::LoopRun()
{
	selectClass.Start(&selectClass);
	WaitForSingleObject(selectClass.GetHandle(), INFINITE);

	while (1)
	{
		/*if (selectClass.GetIsFull())
		{
			isFull = true;
		}
		else
		{
			isFull = false;
			printf("가득차지 않음. \n	");
		}*/
	}
}