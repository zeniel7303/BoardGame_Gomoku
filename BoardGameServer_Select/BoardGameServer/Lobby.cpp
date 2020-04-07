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
	roomManager.Init();

	userNum = 0;

	memset(lobbyUserArray, NULL, 100 * sizeof(userInformation*));
}

void Lobby::LobbyUserUpdata(userInformation* _userInfo, bool _goRoom)
{
	if (_goRoom)
	{
		for (int i = 0; i < userNum; i++)
		{
			if (lobbyUserArray[i]->userID == _userInfo->userID)
			{
				UserUpdate(i);

				return;
			}
		}
	}
	else
	{
		lobbyUserArray[userNum] = _userInfo;

		userNum++;
	}
}

void Lobby::SendChatAll_Lobby(SendChatting* _packet)
{
	for (int i = 0; i < userNum; i++)
	{
		channel->GetSelectClass()->FindSession(lobbyUserArray[i]->socket)
			->Send(_packet, sizeof(SendChatting));
	}
}

void Lobby::SendLobbyUserList(userInformation* _info)
{
	ParticipantsInfo* partiInfo = new ParticipantsInfo;

	partiInfo->cmd = 95;

	int tempUserNum = userNum;
	int tempNum = 0;

	//======================================================================================
	for (int i = 0; i < 100; i++)
	{
		if (lobbyUserArray[i] == NULL) break;

		partiInfo->BelongInfos[i] = *lobbyUserArray[i];
		printf("%s \t", lobbyUserArray[i]->name);

		tempNum++;
	}

	printf("\n");
	//======================================================================================

	/*switch (_packet->page)
	{
	case 1:
		for (int i = 0; i < 20; i++)
		{
			if (lobbyUserArray[i] == NULL) break;

			_packet->BelongInfos[i] = *lobbyUserArray[i];
			printf("%s \t", lobbyUserArray[i]->name);

			tempNum++;
		}
		break;
	case 2:
		if (tempUserNum < 20)
		{
			break;
		}

		for (int i = 20; i < 40; i++)
		{
			if (lobbyUserArray[i] == NULL) break;

			_packet->BelongInfos[i] = *lobbyUserArray[i];
			printf("%s \t", lobbyUserArray[i]->name);

			tempNum++;
		}
		break;
	case 3:
		if (tempUserNum < 40)
		{
			break;
		}

		for (int i = 40; i < 60; i++)
		{
			if (lobbyUserArray[i] == NULL) break;

			_packet->BelongInfos[i] = *lobbyUserArray[i];
			printf("%s \t", lobbyUserArray[i]->name);

			tempNum++;
		}
		break;
	case 4:
		if (tempUserNum < 60)
		{
			break;
		}

		for (int i = 60; i < 80; i++)
		{
			if (lobbyUserArray[i] == NULL) break;

			_packet->BelongInfos[i] = *lobbyUserArray[i];
			printf("%s \t", lobbyUserArray[i]->name);

			tempNum++;
		}
		break;
	case 5:
		if (tempUserNum < 80)
		{
			break;
		}

		for (int i = 80; i < 100; i++)
		{
			if (lobbyUserArray[i] == NULL) break;

			_packet->BelongInfos[i] = *lobbyUserArray[i];
			printf("%s \t", lobbyUserArray[i]->name);

			tempNum++;
		}
		break;
	}*/

	if (tempNum > 0)
	{
		partiInfo->count = tempNum;
		partiInfo->arraySize = partiInfo->count * sizeof(userInformation);
		partiInfo->size = 20 + partiInfo->arraySize;

		channel->GetSelectClass()->FindSession(_info->socket)->Send(partiInfo, partiInfo->size);
	}
}

void Lobby::SendRoomList(userInformation* _info, ReqRoomListData* _packet)
{
	CreatedRoomList *roomListInfo = new CreatedRoomList();

	roomListInfo->cmd = 96;

	int tempRoomNum = roomManager.getRoomCount();
	int tempNum = 0;

	//======================================================================================
	if (tempRoomNum >= 12 * (_packet->page - 1))
	{
		for (int i = 0 * _packet->page; i < 12 * _packet->page; i++)
		{
			if (roomManager.getRoom(i) == NULL) break;

			roomListInfo->roomList[i - (12 * (_packet->page - 1))] = roomManager.getRoom(i)->roomInfo;
			//printf("%d\t", roomManager.getRoom(i)->index);

			tempNum++;
		}
	}
	//======================================================================================

	if (tempNum > 0)
	{
		roomListInfo->count = tempNum;
		roomListInfo->arraySize = roomListInfo->count * sizeof(SendingRoomStruct);
		roomListInfo->size = 20 + roomListInfo->arraySize;

		channel->GetSelectClass()->FindSession(_info->socket)->Send(roomListInfo, roomListInfo->size);
	}
	else if (tempNum == 0)
	{
		roomListInfo->count = 0;
		roomListInfo->arraySize = roomListInfo->count * sizeof(SendingRoomStruct);
		roomListInfo->size = 20 + roomListInfo->arraySize;

		channel->GetSelectClass()->FindSession(_info->socket)->Send(roomListInfo, roomListInfo->size);
	}
}

void Lobby::SendLobbyUserList_(userInformation* _info)
{
	//printf("%s \n", _info->name);

	ParticipantsInfo* partiInfo = new ParticipantsInfo;

	partiInfo->cmd = 95;

	for (int i = 0; i < userNum; i++)
	{
		partiInfo->BelongInfos[i] = *lobbyUserArray[i];
		//printf("%s \t", lobbyUserArray[i]->name);
	}

	//printf("\n");

	partiInfo->cmd = 95;
	partiInfo->count = userNum;
	partiInfo->arraySize = partiInfo->count * sizeof(userInformation);
	partiInfo->size = 20 + partiInfo->arraySize;

	channel->GetSelectClass()->FindSession(_info->socket)->Send(partiInfo, partiInfo->size);
	//send(_info->socket, (char*)partiInfo, partiInfo->size, 0);
}

void Lobby::SendLobbyUserListToOther_(userInformation* _info)
{
	ParticipantsInfo* partiInfo = new ParticipantsInfo;

	for (int i = 0; i < userNum; i++)
	{
		partiInfo->cmd = 95;

		if (lobbyUserArray[i]->userID != _info->userID)
		{
			for (int j = 0; j < userNum; j++)
			{
				partiInfo->BelongInfos[j] = *lobbyUserArray[j];
			}

			partiInfo->cmd = 95;
			partiInfo->count = userNum;
			partiInfo->arraySize = partiInfo->count * sizeof(userInformation);
			partiInfo->size = 20 + partiInfo->arraySize;

			channel->GetSelectClass()->FindSession(lobbyUserArray[i]->socket)->Send(partiInfo, partiInfo->size);
		}
	}
}

void Lobby::pushBackLobbyUser(userInformation* info) 
{ 
	lobbyUserArray[userNum] = info;

	userNum++;
}

void Lobby::LogOut(userInformation* info)
{
	for (int i = 0; i < userNum; i++)
	{
		if (lobbyUserArray[i]->userID == info->userID)
		{
			UserUpdate(i);

			return;
		}
	}
}

void Lobby::UserUpdate(int _num)
{
	if (userNum >= 2)
	{
		for (int i = _num; i < userNum - 1; i++)
		{
			lobbyUserArray[i] = lobbyUserArray[i + 1];
		}

		lobbyUserArray[userNum - 1] = NULL;

		userNum--;
	}
	else
	{
		lobbyUserArray[userNum - 1] = NULL;

		userNum--;
	}
}