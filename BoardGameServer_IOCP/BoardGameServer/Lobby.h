#pragma once
#include <vector>
#include <algorithm>

#include "packet.h"
#include "RoomManager.h"

#include "User.h"
#include "NewRingBuffer.h"

#include "CriticalSectionClass.h"

class Channel;

//=====================================================

//Lobby

//한개의 Channel당 하나당 Lobby 하나
//로비 유저는 벡터로 관리함

//룸 유저 리스트는 어차피 2명이므로 바로바로 모든 유저에게 룸 유저 리스트를 보내준다.

//로비에 유저가 입장,퇴장 시 MakeLobbyUserList() 함수를 호출해 로비 유저를 보내줄 패킷을 만들어준다.
//	+ CriticalSection으로 동시에 여러번 들어오는 것을 방지

//=====================================================

class Lobby
{
private:
	RoomManager				m_roomManager;

	vector<User>			m_vlobbyUserList;

	ParticipantsInfo*		m_participantsInfo;

	Channel*				m_channel;

	CriticalSectionClass	m_locker;

public:
	Lobby();
	~Lobby();

	void Init();
	//로비 참가인원 전체에 채팅 보내기
	void SendChatAll_Lobby(SendChatting* _packet);

	//로비 유저 리스트 보내기
	void SendLobbyUserList(NewRingBuffer* _data, User* _user);
	//룸 리스트 보내기
	void SendRoomList(NewRingBuffer* _data, User* _user, ReqRoomListData* _packet);

	void PushBackLobbyUser(User* _user);
	void EraseLobbyUser(User* _user);

	void MakeLobbyUserList();

	void SetChannelData(Channel* _channel) { m_channel = _channel; }

	Channel* getChannelClass() { return m_channel; }
	RoomManager* getRoomManager() { return &m_roomManager; }
};