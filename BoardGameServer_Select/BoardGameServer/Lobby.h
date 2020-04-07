#pragma once
#include <vector>

#include "packet.h"
#include "RoomManager.h"

class Channel;

//=====================================================

//Lobby

//한개의 SelectClass 하나당 Lobby 하나 둘 것 (Channel 구현 시)
//로비 유저는 배열로 관리함
//SelectClass와는 상호참조 중임

//룸 유저 리스트는 어차피 2명이므로 바로바로 모든 유저에게 룸 유저 리스트를 보내준다.

//SendLobbyUserListToOther 함수는 의미가 있나? 어차피 현재는 새로고침을 누를 경우에만 갱신시켜주므로 보류함.
//					 (자동으로 보내주게되면 사용할듯)

// =========== 테스트 아직 부족함 =============
//로비 유저 리스트 and 룸 리스트는 클라에서 새로고침 요구시에만 새로 리스트를 보내준다.
//                   (자동으로 보내주는건 아마 로비 유저 리스트에만 적용할 수도 있음)

//ps
//같은 함수명인데 뒤에 _붙은건 과거버전

//=====================================================

class Lobby
{
private:
	RoomManager				roomManager;

	userInformation*		lobbyUserArray[100];
	int						userNum;

	vector<RoomStruct>		vRoomList;

	Channel*			channel;

public:
	Lobby();
	~Lobby();

	void Init();
	void LobbyUserUpdata(userInformation* _userInfo, bool _goRoom);
	//로비 참가인원 전체에 채팅 보내기
	void SendChatAll_Lobby(SendChatting* _packet);

	//로비 유저 리스트 보내기
	void SendLobbyUserList(userInformation* _info);
	//룸 리스트 보내기
	void SendRoomList(userInformation* _info, ReqRoomListData* _packet);

	//=============================================================================

	//로비 참가인원 정보 보내기
	void SendLobbyUserList_(userInformation* _info);
	void SendLobbyUserListToOther_(userInformation* _info);

	//=============================================================================

	void pushBackLobbyUser(userInformation* _session);
	void LogOut(userInformation* info);
	void UserUpdate(int _num);

	void SetChannelData(Channel* _channel) { channel = _channel; }

	userInformation getLobbyUser(int num) { return *lobbyUserArray[num - 1]; }

	Channel* getChannelClass() { return channel; }
	RoomManager* getRoomManager() { return &roomManager; }

	void ShowLobbyUserList()
	{
		printf("----------------------- \n");
		printf("Lobby User list \n");

		for (int i = 0; i < userNum; i++)
		{
			printf("%s \n", lobbyUserArray[i]->name);
		}

		printf("----------------------- \n");
	}
};