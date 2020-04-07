#pragma once
#include <vector>
#include <algorithm>
#include "packet.h"

#include "User.h"
#include "NewRingBuffer.h"

#include "ObjectPool.h"
#include "CriticalSectionClass.h"

#define ROOMMAX 50

using namespace std;

//=====================================================

//RoomManager

//각 룸의 정보는 RoomStruct 구조체에 존재

//중간에 삭제된 방의 번호는 vector로 관리해둠.
//	삭제된 방 번호 저장해둔 이유 = 중간에서 삭제된 방 번호를 저장해둬서 룸 생성할 시 할당하기 편함

//게임이 끝나면 바로 방을 없앤다.

//SendRoomUserListToOther_가 두개인 이유
//위의 함수는 직접 방 번호를 가져오는 것은 이미 유저가 나가버려 유저의 룸 번호를 가져올 수 없어서 사용 - 유저가 나갈때 룸 번호만 따로 불러와서 제거해주고있다.
//아래의 함수는 유저가 방에서 나간 상황이 아니라 유저 정보내에서 방 번호를 가져올 수 있으므로 사용

//20191010 - 수정 or 추가 구현해야할 사항
//1. 현재 룸에서 게임이 진행 중인지를 저장도 안하고 클라에서 표기도 안하고 있었음.							(표기완료)
// # 어디서 게임 진행이 끝난지를 알려줘야할까? 현재 게임 끝난 후 그냥 ExitRoom으로 바로 진행 중 #
//2. 게임 내에서 턴 마다 제한 시간이 없음. 없어도 되긴하는데 실제 게임에선 필요한 기능이므로...				(하지않았음. 당장 필요는 없다.)

//CreateRoom, JoinRoom, ExitRoom은 동시에 여러번 접근할 가능성이 있으므로 전부 CriticalSection으로 막아두었다.

//=====================================================

class RoomManager : public ObjectPool<RoomStruct<User>>
{
private:
	//vector<RoomStruct<User>> vRoomList;

	RoomStruct<User>*		m_roomStruct[ROOMMAX];
	//중간에 빈 방의 번호 넣는 벡터
	vector<int>				m_roomNum;

	int						m_roomCount;

	//==================================================

	SendChatting*			m_sendNoRoom;

	CriticalSectionClass	m_locker;

	//==================================================

	//각 페이지마다 1개씩
	CreatedRoomList*		m_roomListInfo_1;
	CreatedRoomList*		m_roomListInfo_2;
	CreatedRoomList*		m_roomListInfo_3;
	CreatedRoomList*		m_roomListInfo_4;
	CreatedRoomList*		m_roomListInfo_5;

public:
	RoomManager();
	~RoomManager();
	
	void Init();

	RoomStruct<User>* FindRoom(int _roomNum);

	//룸 내부 채팅 전달
	void SendChattAll_Room(int _roomNum, SendChatting* _packet);

	//룸 참가인원 정보 뿌리기
	void SendRoomUserList(NewRingBuffer* _data, User* _user);
	//룸 내 다른 참가인원들에게 참가인원 정보 뿌리기
	void SendRoomUserListToOther(NewRingBuffer* _data, int _roomNum);
	void SendRoomUserListToOther(NewRingBuffer* _data, User* _user);

	void RoomUserUpdate(User* _user);

	bool CreateRoom(NewRingBuffer* _data, User* _user);

	bool JoinRoom(NewRingBuffer* _data, User* _user, int tempNum);

	bool ExitRoom(NewRingBuffer* _data, User* _user);
	void LeaveRoom(User* _user);
	void DestroyRoom(User* _user);

	bool ReadyCheck(RoomStruct<User>* _tempRoom);
	void GomokuGridPacket(NewRingBuffer* _data, userInformation* _tempInfo, char* _packet);
	void GomokuResultPacket(NewRingBuffer* _data, userInformation* _tempInfo, char* _packet);

	void RoomsUpdate(int _num);
	void SortAscending();

	void MakeRoomList();

	int GetRoomCount() { return m_roomCount; }
	RoomStruct<User>* GetRoom(int num) { return m_roomStruct[num]; }

	CreatedRoomList* GetCreatedRoomList_1() { return m_roomListInfo_1; }
	CreatedRoomList* GetCreatedRoomList_2() { return m_roomListInfo_2; }
	CreatedRoomList* GetCreatedRoomList_3() { return m_roomListInfo_3; }
	CreatedRoomList* GetCreatedRoomList_4() { return m_roomListInfo_4; }
	CreatedRoomList* GetCreatedRoomList_5() { return m_roomListInfo_5; }
};