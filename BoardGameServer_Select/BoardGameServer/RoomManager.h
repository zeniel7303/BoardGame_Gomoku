#pragma once
#include <vector>
#include <algorithm>
#include "packet.h"

//#include "Room.h"

#define ROOMMAX 50

using namespace std;

//=====================================================

//RoomManager

//각 룸의 정보는 RoomStruct 구조체에 존재

//중간에 삭제된 방의 번호는 vector로 관리해둠.
//	삭제된 방 번호 저장해둔 이유 = 중간에서 삭제된 방 번호를 저장해둬서 룸 생성할 시 할당하기 편함

//SelectClass와 상호참조

//게임이 끝나면 바로 방을 없앤다.

//SendRoomUserListToOther_가 두개인 이유
//위의 함수는 직접 방 번호를 가져오는 것은 이미 유저가 나가버려 유저의 룸 번호를 가져올 수 없어서 사용
//아래의 함수는 유저가 방에서 나간 상황이 아니라 유저 정보내에서 방 번호를 가져올 수 있으므로 사용
// # 최근에 든 생각인데 굳이 이렇게 할 필요가 있나? # - 수정 예정

//20191010 - 수정 or 추가 구현해야할 사항
//1. 현재 룸에서 게임이 진행 중인지를 저장도 안하고 클라에서 표기도 안하고 있었음.
// # 어디서 게임 진행이 끝난지를 알려줘야할까? 현재 게임 끝난 후 그냥 ExitRoom으로 바로 진행 중 #
//2. 게임 내에서 턴 마다 제한 시간이 없음. 없어도 되긴하는데 실제 게임에선 필요한 기능이므로...

//ps
//같은 함수명인데 뒤에 _붙은건 과거버전

//=====================================================

class Channel;

class RoomManager
{
private:
	RoomStruct* roomStruct[ROOMMAX];
	//중간에 빈 방의 번호 넣는 벡터
	vector<int> roomNum;

	int roomCount = 0;

	Channel* channel;

	//==================================================

	SendChatting* sendNoRoom;

public:
	RoomManager();
	~RoomManager();
	
	void Init();

	//룸 내부 채팅 전달
	void SendChattAll_Room(int _roomNum, SendChatting* _packet);

	void SendRoomUserList(userInformation* _info, ParticipantsInfo* _packet);

	// ========================================================

	//룸 참가인원 정보 뿌리기
	void SendRoomUserList_(userInformation* _info);
	//룸 내 다른 참가인원들에게 참가인원 정보 뿌리기
	void SendRoomUserListToOther_(int _roomNum);
	void SendRoomUserListToOther_(userInformation* _info);

	// ========================================================

	void RoomUserUpdate(userInformation* _info);

	bool CreateRoom(userInformation* _info);

	bool JoinRoom(userInformation* _info, int tempNum);

	bool ExitRoom(userInformation* _info);
	void LeaveRoom(userInformation* _info);
	void DestroyRoom(userInformation* _info);

	void ReadyCheck(RoomStruct* _tempRoom);
	void GomokuGridPacket(userInformation* _tempInfo, char* _packet);
	void GomokuResultPacket(userInformation* _tempInfo, char* _packet);

	void RoomsUpdate(int _num);
	void SortAscending();

	void SetChannelData(Channel* _channel) { channel = _channel; }

	int getRoomCount() { return roomCount; }

	//RoomStruct* getRooms() { return roomStruct; }
	RoomStruct* getRoom(int num) { return roomStruct[num]; }
	int getRoomIndex(int num) { return roomStruct[num]->roomInfo.index; }

	/*Room getRoom(int num) { return *room[num]; }
	int getRoomIndex(int i) { return room[i]->getIndex(); }*/
};