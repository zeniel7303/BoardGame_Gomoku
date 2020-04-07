#include "RoomManager.h"
#include "Channel.h"

RoomManager::RoomManager()
{
}

RoomManager::~RoomManager()
{
}

void RoomManager::Init()
{
	memset(roomStruct, NULL, ROOMMAX * sizeof(RoomStruct*));

	roomCount = 0;

	//미리 오류 메세지 만들어두기
	char tempCharArray[255] = "참가할 방이 존재하지 않습니다. 새로고침 해주세요.";
	sendNoRoom = new SendChatting();

	sendNoRoom->cmd = 91;
	strncpy(sendNoRoom->name, "System",sizeof("System"));
	sendNoRoom->nameSize = sizeof(sendNoRoom->name);
	strcpy(sendNoRoom->message, tempCharArray);
	sendNoRoom->size = sizeof(sendNoRoom);
}

//룸 내부 채팅 전달
void RoomManager::SendChattAll_Room(int _roomNum, SendChatting* _packet)
{
	for (int i = 0; i < roomCount; i++)
	{
		if (roomStruct[i]->roomInfo.index == _roomNum)
		{
			for (int j = 0; j < roomStruct[i]->roomInfo.count; j++)
			{
				channel->GetSelectClass()->FindSession(roomStruct[i]->userInfo_inRoom[j].socket)
					->Send(_packet, sizeof(SendChatting));
			}

			break;
		}
	}
}

//미사용중
void RoomManager::SendRoomUserList(userInformation* _info, ParticipantsInfo* _packet)
{
	for (int i = 0; i < roomCount; i++)
	{
		if (roomStruct[i]->roomInfo.index == _info->roomNum)
		{
			for (int j = 0; j < roomStruct[i]->roomInfo.count; j++)
			{
				_packet->BelongInfos[j] = roomStruct[i]->userInfo_inRoom[j];
			}

			_packet->cmd = 95;
			_packet->count = roomStruct[i]->roomInfo.count;
			_packet->arraySize = _packet->count * sizeof(userInformation);
			_packet->size = 20 + _packet->arraySize;

			//보냄
			channel->GetSelectClass()->FindSession(_info->socket)->Send(_packet, _packet->size);
			//send(_info->socket, (char*)partiInfo, partiInfo->size, 0);

			return;
		}
	}
}

//룸 참가인원 정보 뿌리기
void RoomManager::SendRoomUserList_(userInformation* _info)
{
	ParticipantsInfo* partiInfo = new ParticipantsInfo;

	for (int i = 0; i < roomCount; i++)
	{
		if (roomStruct[i]->roomInfo.index == _info->roomNum)
		{
			for (int j = 0; j < roomStruct[i]->roomInfo.count; j++)
			{
				partiInfo->BelongInfos[j] = roomStruct[i]->userInfo_inRoom[j];
			}

			partiInfo->cmd = 95;
			partiInfo->count = roomStruct[i]->roomInfo.count;
			partiInfo->arraySize = partiInfo->count * sizeof(userInformation);
			partiInfo->size = 20 + partiInfo->arraySize;

			//보냄
			channel->GetSelectClass()->FindSession(_info->socket)->Send(partiInfo, partiInfo->size);

			return;
		}
	}
}

void RoomManager::SendRoomUserListToOther_(int _roomNum)
{
	ParticipantsInfo* partiInfo = new ParticipantsInfo;

	if (_roomNum > 0)
	{
		for (int i = 0; i < roomCount; i++)
		{
			if (roomStruct[i]->roomInfo.index == _roomNum)
			{
				for (int j = 0; j < roomStruct[i]->roomInfo.count; j++)
				{
					partiInfo->BelongInfos[j] = roomStruct[i]->userInfo_inRoom[j];
				}

				partiInfo->cmd = 95;
				partiInfo->count = roomStruct[i]->roomInfo.count;
				partiInfo->arraySize = partiInfo->count * sizeof(userInformation);
				partiInfo->size = 20 + partiInfo->arraySize;

				for (int j = 0; j < roomStruct[i]->roomInfo.count; j++)
				{
					channel->GetSelectClass()->FindSession(roomStruct[i]->userInfo_inRoom[j].socket)
						->Send(partiInfo, partiInfo->size);
				}
			}
		}
	}
}

//룸 내 다른 참가인원들에게 참가인원 정보 뿌리기
void RoomManager::SendRoomUserListToOther_(userInformation* _info)
{
	ParticipantsInfo* partiInfo = new ParticipantsInfo;

	if (_info->roomNum > 0)
	{
		for (int i = 0; i < roomCount; i++)
		{
			if (roomStruct[i]->roomInfo.index == _info->roomNum)
			{
				for (int j = 0; j < roomStruct[i]->roomInfo.count; j++)
				{
					partiInfo->BelongInfos[j] = roomStruct[i]->userInfo_inRoom[j];
				}

				partiInfo->cmd = 95;
				partiInfo->count = roomStruct[i]->roomInfo.count;
				partiInfo->arraySize = partiInfo->count * sizeof(userInformation);
				partiInfo->size = 20 + partiInfo->arraySize;

				for (int j = 0; j < roomStruct[i]->roomInfo.count; j++)
				{
					if (roomStruct[i]->userInfo_inRoom[j].userID != _info->userID)
					{
						channel->GetSelectClass()->FindSession(roomStruct[i]->userInfo_inRoom[j].socket)
							->Send(partiInfo, partiInfo->size);
					}
				}
			}
		}
	}
}

void RoomManager::RoomUserUpdate(userInformation* _info)
{
	for (int i = 0; i < roomCount; i++)
	{
		if (_info->roomNum == roomStruct[i]->roomInfo.index)
		{
			for (int j = 0; j < roomStruct[i]->roomInfo.count; j++)
			{
				if (roomStruct[i]->userInfo_inRoom[j].socket == _info->socket)
				{
					roomStruct[i]->userInfo_inRoom[j].isReady = _info->isReady;

					this->ReadyCheck(roomStruct[i]);

					return;
				}
			}
		}
	}
}

bool RoomManager::CreateRoom(userInformation* _info)
{
	if (roomCount >= MAX)
	{
		printf("[방 생성 제한 갯수에 걸림] \n");

		return false;
	}

	for (int i = 0; i < MAX; i++)
	{
		if (roomStruct[i] == NULL)
		{
			roomCount++;

			roomStruct[i] = new RoomStruct();

			//중간에 빈 룸 번호가 있으면 할당
			if (roomNum.size() > 0)
			{
				roomStruct[i]->roomInfo.index = roomNum[0];
				roomNum.erase(roomNum.begin());
			}
			//아니면 계속 쭉
			else
			{
				roomStruct[i]->roomInfo.index = i + 1;
			}

			roomStruct[i]->roomInfo.count = 1;
			roomStruct[i]->roomInfo.complement = 2;
			roomStruct[i]->userInfo_inRoom[0] = *_info;
			roomStruct[i]->roomInfo.isPlaying = false;

			UserData* data = new UserData();

			data->cmd = 92;

			printf("[Room %d Created] \n", roomStruct[i]->roomInfo.index);
			_info->roomNum = roomStruct[i]->roomInfo.index;

			SortAscending();

			data->roomNum = roomStruct[i]->roomInfo.index;
			data->size = sizeof(UserData);

			channel->GetSelectClass()->FindSession(_info->socket)->Send(data, sizeof(UserData));
			//send(_info->socket, (char*)data, sizeof(UserData), 0);

			return true;
		}
	}

	printf("[방 생성 오류] \n");

	return false;
}

bool RoomManager::JoinRoom(userInformation* _info, int tempNum)
{
	//참여할 방이 없다.
	if (tempNum == NULL)
	{
		printf("[참여할 방이 없음. (NULL 들어옴)]\n");

		return false;
	}

	UserData* data = new UserData();

	data->cmd = 94;

	int tempRoomIndex = 0;
	for (int i = 0; i < roomCount; i++)
	{
		if (roomStruct[i]->roomInfo.index == tempNum && roomStruct[i]->roomInfo.count < roomStruct[i]->roomInfo.complement 
			&& !roomStruct[i]->roomInfo.isPlaying)
		{
			roomStruct[i]->roomInfo.count = roomStruct[i]->roomInfo.count + 1;
			roomStruct[i]->userInfo_inRoom[1] = *_info;

			printf("[Join %d Room]\n", roomStruct[i]->roomInfo.index);
			_info->roomNum = roomStruct[i]->roomInfo.index;
			
			data->roomNum = roomStruct[i]->roomInfo.index;
			data->size = sizeof(UserData);

			channel->GetSelectClass()->FindSession(_info->socket)->Send(data, sizeof(UserData));

			return true;
		}
	}

	printf("[해당 방 번호가 없음]\n");

	channel->GetSelectClass()->FindSession(_info->socket)->Send(sendNoRoom, sizeof(SendChatting));

	return false;
}

bool RoomManager::ExitRoom(userInformation* _info)
{
	if (_info->roomNum == NULL)
	{
		printf("[나갈 방이 없음]\n");

		return false;
	}

	UserData* data = new UserData();

	data->cmd = 93;

	bool tmp = false;
	//해당 방에 유저가 더 있는지 검색
	for (int i = 0; i < roomCount; i++)
	{
		if (roomStruct[i]->roomInfo.index == _info->roomNum)
		{
			for (int j = 0; j < roomStruct[i]->roomInfo.count; j++)
			{
				if (roomStruct[i]->userInfo_inRoom[j].userID != _info->userID)
				{
					tmp = true;

					break;
				}
			}
			break;
		}
	}

	//다른 사람도 있으니 방은 냅두고 퇴장만
	if (tmp)
	{
		printf("[Left Room %d]\n", _info->roomNum);
		_info->isReady = false;

		this->LeaveRoom(_info);
		_info->roomNum = 0;
		
		data->roomNum = 0;
		data->size = sizeof(UserData);

		channel->GetSelectClass()->FindSession(_info->socket)->Send(data, sizeof(UserData));

		return true;
	}
	//내가 마지막이였으니 방 폭파시키고 퇴장
	else
	{
		this->DestroyRoom(_info);

		printf("[Room %d Destroyed]\n", _info->roomNum);
		_info->roomNum = 0;
		_info->isReady = false;

		data->roomNum = 0;
		data->size = sizeof(&data);

		channel->GetSelectClass()->FindSession(_info->socket)->Send(data, sizeof(UserData));

		return true;
	}
}

void RoomManager::LeaveRoom(userInformation* tempInfo)
{
	for (int i = 0; i < roomCount; i++)
	{
		if (tempInfo->roomNum == roomStruct[i]->roomInfo.index)
		{
			roomStruct[i]->roomInfo.count = roomStruct[i]->roomInfo.count - 1;

			for (int j = 0; j < roomStruct[i]->roomInfo.count; j++)
			{
				if (roomStruct[i]->userInfo_inRoom[j].socket == tempInfo->socket)
				{
					if (j == 0)
					{
						roomStruct[i]->userInfo_inRoom[0] = roomStruct[i]->userInfo_inRoom[1];

						memset(&roomStruct[i]->userInfo_inRoom[1], 0, sizeof(roomStruct[i]->userInfo_inRoom[1]));
					}
					else if (j == 1)
					{
						memset(&roomStruct[i]->userInfo_inRoom[1], 0, sizeof(roomStruct[i]->userInfo_inRoom[1]));
					}

					return;
				}
			}
		}
	}
}

void RoomManager::DestroyRoom(userInformation* tempInfo)
{
	//printf("Start Destroying Room \n");

	for (int i = 0; i < MAX; i++)
	{
		if (tempInfo->roomNum == roomStruct[i]->roomInfo.index)
		{
			RoomsUpdate(i);

			//printf("End Destroying Room\n");
			return;
		}
	}
}

void RoomManager::ReadyCheck(RoomStruct* _tempRoom)
{
	bool isAllReady = true;

	if (_tempRoom->roomInfo.count < _tempRoom->roomInfo.complement) isAllReady = false;

	for (int i = 0; i < _tempRoom->roomInfo.count; i++)
	{
		if (!_tempRoom->userInfo_inRoom[i].isReady) isAllReady = false;
	}

	if (isAllReady)
	{
		InGameUserInfo* temp = (InGameUserInfo*)malloc(2);

		for (int i = 0; i < _tempRoom->roomInfo.count; i++)
		{
			if (i == 0)
			{
				UserData* data = new UserData();
				data->cmd = 198;
				data->size = sizeof(UserData);

				channel->GetSelectClass()->FindSession(_tempRoom->userInfo_inRoom[i].socket)
					->Send(data, sizeof(UserData));

				temp[0].user = _tempRoom->userInfo_inRoom[i];
				temp[0].order = 0;
			}
			else if (i == 1)
			{
				UserData* data = new UserData();
				data->cmd = 199;
				data->size = sizeof(UserData);

				channel->GetSelectClass()->FindSession(_tempRoom->userInfo_inRoom[i].socket)
					->Send(data, sizeof(UserData));

				temp[1].user = _tempRoom->userInfo_inRoom[i];
				temp[1].order = 1;
			}
		}

		//플레이어가 전부 Ready가 됐으므로 룸은 플레이 중으로 전환
		_tempRoom->roomInfo.isPlaying = true;
	}
}

void RoomManager::GomokuGridPacket(userInformation* _tempRoom, char* _packet)
{
	GomokuGrid* data = (GomokuGrid*)_packet;

	for (int i = 0; i < roomCount; i++)
	{
		if (roomStruct[i]->roomInfo.index == _tempRoom->roomNum)
		{
			for (int j = 0; j < roomStruct[i]->roomInfo.count; j++)
			{
				if (_tempRoom->userID != roomStruct[i]->userInfo_inRoom[j].userID)
				{
					data->cmd = 200;

					channel->GetSelectClass()->FindSession(roomStruct[i]->userInfo_inRoom[j].socket)
						->Send(data, sizeof(GomokuGrid));

					return;
				}
			}
		}
	}
}

void RoomManager::GomokuResultPacket(userInformation* _tempRoom, char* _packet)
{
	GomokuResult* data = (GomokuResult*)_packet;

	for (int i = 0; i < roomCount; i++)
	{
		if (roomStruct[i]->roomInfo.index == _tempRoom->roomNum)
		{
			for (int j = 0; j < roomStruct[i]->roomInfo.count; j++)
			{
				if (_tempRoom->userID != roomStruct[i]->userInfo_inRoom[j].userID)
				{
					data->cmd = 201;

					channel->GetSelectClass()->FindSession(roomStruct[i]->userInfo_inRoom[j].socket)
						->Send(data, sizeof(GomokuGrid));

					return;
				}
			}
		}
	}
}

void RoomManager::RoomsUpdate(int _num)
{
	if (_num < roomCount - 1)
	{
		roomNum.push_back(roomStruct[_num]->roomInfo.index);
		//printf("%d \n", roomStruct[_num]->index);
	}

	if (roomCount >= 2)
	{
		for (int i = _num; i < roomCount - 1; i++)
		{
			roomStruct[i] = roomStruct[i + 1];
		}

		roomStruct[roomCount - 1] = NULL;

		roomCount--;

		this->SortAscending();
	}
	else
	{
		roomStruct[roomCount - 1] = NULL;
		roomCount--;
	}

	/*for (int i = 0; i < roomCount; i++)
	{
		printf("%d \t", roomStruct[i]->index);
	}*/
}

void RoomManager::SortAscending()
{
	for (int i = 0; i < roomCount - 1; i++)
	{
		for (int j = i + 1; j < roomCount; j++)
		{
			if (roomStruct[i]->roomInfo.index > roomStruct[j]->roomInfo.index)
			{
				RoomStruct *temp;
				temp = roomStruct[j];
				roomStruct[j] = roomStruct[i];
				roomStruct[i] = temp;
			}
		}
	}

	/*for (int i = 0; i < roomCount; i++)
	{
		printf("%d\t", roomStruct[i]->index);
	}*/

	//printf("\n");
}