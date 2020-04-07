#pragma once

#pragma warning(disable:4996)

#include "packet.h"
#include "RingBuffer.h"

/*
// Lee.ks - 190515
이 클래스 역활?????????

roomManager ( why ????? ) 
userinfo -- sessionmanager 가 가지고 있는 session buffer 

 멤버 변수, 지역변수 구분이 안됨
*/

//세션은 로그인 정보 유지를 위해 사용한다.
//
//서버 부하 테스트 관련
//http://www.devkorea.co.kr/bbs/board.php?bo_table=m03_qna&wr_id=98817

//=====================================================

//Session

//Server에서부터 가져온 Lobby 데이터 활용 해 Lobby와 RoomManager 접근

//## Send함수
//Lobby와 RoomManager에서 SelectClass로 다시 접근해 Session을 찾아 Send함수를 이용해 Send함

//=====================================================

template<typename T>
class Session
{
protected:
	//해당 세션의 유저 정보
	userInformation info;

	int				recvByte;
	char			sendData[5000];
	char*			savedBuffer;
	int				savedPosition;

	RingBuffer*		ringBuffer;

private:

public:
	Session()
	{
		ringBuffer = new RingBuffer();
		ringBuffer->Init(1000);

		savedBuffer = nullptr;
		savedPosition = 0;
	}

	~Session()
	{

	}

	virtual void Init(T* _t) = 0;
	virtual void Reset() = 0;
	virtual bool Recv() = 0;
	virtual bool Parsing() = 0;
	virtual void Send(void* _data, int _size) = 0;

	userInformation getSessionInfo() { return info; }
	RingBuffer* getRingBuffer() { return ringBuffer; }
};
