#pragma once
#include "ThreadClass.h"

#include "Channel.h"

#define CHANNELNUM 3

//오브젝트 풀 관련
//https://developstudy.tistory.com/44

//=====================================================

//ChannelManager

//구현에 대한 것
//채널마다 포트번호 다르게 해서 사용해야한다고 한다. - HOW? 아직도 감이 잘 안잡힘
//		(현재 포트번호를 Server의 Init함수에서 할당중임.)
//이걸 빨리 구현해야 CS에 대한 공부를 할 수 있다.

//ObjectPool로 채널 생성 후 채널 필요할 때마다 꺼내 쓰기 (현재 채널 리스트는 3개로 고정시켜둬서 미사용중)

//ThreadClass를 상속받아 Thread를 돌리면서 Channel 생성 혹은 폐기가 필요한지 상시 체크 (이것또한 고정 중이라 미 사용중)

//=====================================================

class ChannelManager /*: public ObjectPool<Channel> , public ThreadClass<ChannelManager>*/
{
private:
	Channel m_channelArray[CHANNELNUM];

public:
	ChannelManager();
	~ChannelManager();

	void Init();
};

