#include "ChannelManager.h"

ChannelManager::ChannelManager()
{
	//portNum = 30002;

	//portNum++;

	if (!channelArray[0].Init(30002))
	{
		printf("Channel Init Error \n");
	}

	handleArray[0] = channelArray[0].GetHandle();

	if(!channelArray[1].Init(30003))
	{
		printf("Channel Init Error \n");
	}

	handleArray[1] = channelArray[1].GetHandle();

	if (!channelArray[2].Init(30004))
	{
		printf("Channel Init Error \n");
	}

	handleArray[2] = channelArray[2].GetHandle();

	channelNum = CHANNELNUM;
}

ChannelManager::~ChannelManager()
{
}

void ChannelManager::CloseChannels()
{
	closesocket(channelArray[0].GetListenSocket());
	closesocket(channelArray[1].GetListenSocket());
	closesocket(channelArray[2].GetListenSocket());

	//WSACleanup();
}

//테스팅용
void ChannelManager::ShowChannelList()
{
	for (int i = 0; i < channelNum; i++)
	{
		printf("%d, ", i);
	}

	printf("\n");
}


//쓰레드 부분
void ChannelManager::LoopRun()
{
	channelArray[0].Start(&channelArray[0]);
	channelArray[1].Start(&channelArray[1]);
	channelArray[2].Start(&channelArray[2]);

	WaitForMultipleObjects(CHANNELNUM, handleArray, true, INFINITE);

	while (1)
	{
		
	}

	/*
	while (1)
	{
		for (int i = 0; i < channelNum; i++)
		{
			//특정 채널이 꽉찼다.
			if (channelArray[i]->GetIsFull())
			{
				//채널 하나 더 추가
				channelArray[channelNum] = PopObject();
				channelArray[channelNum]->Init(portNum);
				channelArray[channelNum]->Start(channelArray[channelNum]);

				portNum++;

				//포문 탈출
				break;
			}
		}

		for (int i = 0; i < channelNum; i++)
		{
			//특정 채널이 비었다 && 마지막 채널이 아니다.
			if (channelArray[i]->GetIsEmpty() && i != 0)
			{
				//채널을 오브젝트 풀에 다시 반납
				ReturnObject(channelArray[i]);

				//만약 중간 채널이 비어지게 된다면 중간에 빈 채널을 채워야한다.
				while (i == channelNum - 1)
				{
					//삭제된 채널 뒷 부분의 채널들을
					//하나씩 빈 공간을 채우기 위해 앞으로 당긴다.
					channelArray[i] = channelArray[i + 1];
					i++;
				}

				//마지막 부분은 제거
				channelArray[i] = NULL;
				//가지고있는 채널의 갯수가 한개 줄었다.
				channelNum--;
				
				//포문 탈출
				break;
			}
		}

		ShowChannelList();
	}
	*/
}