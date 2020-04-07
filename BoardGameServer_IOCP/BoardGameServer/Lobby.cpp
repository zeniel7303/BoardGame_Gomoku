#include "Lobby.h"
#include "Channel.h"

Lobby::Lobby()
{
}

Lobby::~Lobby()
{
}

void Lobby::Init()
{
	//로비 참가 유저 리스트 패킷
	m_participantsInfo = new ParticipantsInfo();

	//룸 매니저
	m_roomManager.Init();
}

void Lobby::SendChatAll_Lobby(SendChatting* _packet)
{
	for (int i = 0; i < m_vlobbyUserList.size(); i++)
	{
		m_vlobbyUserList[i].Send(reinterpret_cast<char*>(_packet), sizeof(SendChatting));
	}
}

void Lobby::SendLobbyUserList(NewRingBuffer* _data, User* _user)
{
	if (m_vlobbyUserList.size() > 0)
	{
		_user->Send(reinterpret_cast<char*>(m_participantsInfo), m_participantsInfo->size);
	}
}

void Lobby::SendRoomList(NewRingBuffer* _data, User* _user, ReqRoomListData* _packet)
{
	CreatedRoomList *roomListInfo = (CreatedRoomList*)_data->GetWritePoint();

	roomListInfo->cmd = 96;

	m_locker.EnterLock();

	int tempRoomNum = m_roomManager.GetRoomCount();
	int tempNum = 0;

	//======================================================================================
	if (tempRoomNum > 12 * (_packet->page - 1))
	{
		for (int i = 0 * _packet->page; i < 12 * _packet->page; i++)
		{
			if (m_roomManager.GetRoom(i) == NULL) break;

			roomListInfo->roomList[i - (12 * (_packet->page - 1))] = m_roomManager.GetRoom(i)->roomInfo;

			tempNum++;
		}
	}
	//======================================================================================
	/*switch (_packet->page)
	{
	case 1:
		roomListInfo = m_roomManager.GetCreatedRoomList_1();
		break;
	case 2:
		roomListInfo = m_roomManager.GetCreatedRoomList_2();
		break;
	case 3:
		roomListInfo = m_roomManager.GetCreatedRoomList_3();
		break;
	case 4:
		roomListInfo = m_roomManager.GetCreatedRoomList_4();
		break;
	case 5:
		roomListInfo = m_roomManager.GetCreatedRoomList_5();
		break;
	default:
		//잘못들어오면 무시
		break;
	}*/
	//======================================================================================

	if (tempNum > 0)
	{
		roomListInfo->count = tempNum;
		roomListInfo->arraySize = roomListInfo->count * sizeof(SendingRoomStruct);
		roomListInfo->size = 20 + roomListInfo->arraySize;
	}
	else if (tempNum == 0)
	{
		roomListInfo->count = 0;
		roomListInfo->arraySize = roomListInfo->count * sizeof(SendingRoomStruct);
		roomListInfo->size = 20 + roomListInfo->arraySize;
	}

	m_locker.LeaveLock();

	//_data->Write(roomListInfo->size);

	_user->Send(reinterpret_cast<char*>(roomListInfo), roomListInfo->size);
}

void Lobby::PushBackLobbyUser(User* _user) 
{ 
	m_locker.EnterLock();

	m_vlobbyUserList.push_back(*_user);

	MakeLobbyUserList();

	m_locker.LeaveLock();
}

void Lobby::EraseLobbyUser(User* _user)
{
	m_locker.EnterLock();

	for (int i = 0; i < m_vlobbyUserList.size(); i++)
	{
		m_locker.EnterLock();

		if (m_vlobbyUserList[i].GetSessionInfo().userID == _user->GetSessionInfo().userID)
		{
			m_vlobbyUserList.erase(m_vlobbyUserList.begin() + i);

			MakeLobbyUserList();

			break;
		}
	}

	m_locker.LeaveLock();
}

void Lobby::MakeLobbyUserList()
{
	for (int i = 0; i < m_vlobbyUserList.size(); i++)
	{
		m_participantsInfo->BelongInfos[i] = m_vlobbyUserList[i].GetSessionInfo();
	}

	m_participantsInfo->cmd = 95;
	m_participantsInfo->count = m_vlobbyUserList.size();
	m_participantsInfo->arraySize = m_participantsInfo->count * sizeof(userInformation);
	m_participantsInfo->size = 20 + m_participantsInfo->arraySize;
}