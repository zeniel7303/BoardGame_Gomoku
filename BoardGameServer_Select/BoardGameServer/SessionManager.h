#pragma once

#include <map>

#include "Lobby.h"
#include "RoomManager.h"

#include "Session.h"

#define MAX 100

using namespace std;

class SessionManager
{
private:
	Lobby* lobby;

public:
	SessionManager();
	~SessionManager();

	void Init();

	Lobby* getLobby() { return lobby; }
};