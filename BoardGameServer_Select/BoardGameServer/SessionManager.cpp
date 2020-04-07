#include "SessionManager.h"

SessionManager::SessionManager()
{
}

SessionManager::~SessionManager()
{
}

void SessionManager::Init()
{
	lobby = new Lobby();
	lobby->Init();
}