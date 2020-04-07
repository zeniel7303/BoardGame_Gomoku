#include "ChannelManager.h"

ChannelManager::ChannelManager()
{
	
}

ChannelManager::~ChannelManager()
{
}

void ChannelManager::Init()
{
	if (!m_channelArray[0].Init(30002))
	{
		printf("Channel Init Error \n");
	}

	if (!m_channelArray[1].Init(30003))
	{
		printf("Channel Init Error \n");
	}

	if (!m_channelArray[2].Init(30004))
	{
		printf("Channel Init Error \n");
	}

	IOCPCLASS->Init();
}

//쓰레드 부분
//void ChannelManager::LoopRun()
//{
//	while (1)
//	{
//		Sleep(1);
//	}
//}