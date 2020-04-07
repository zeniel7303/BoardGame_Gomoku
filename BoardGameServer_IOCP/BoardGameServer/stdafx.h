#pragma once

#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <process.h>

#include <map>
#include <list>
#include <queue>
#include <vector>
#include <algorithm>

#include "packet.h"

#include "IOCPClass.h"

using namespace std;

#define IOCPCLASS IOCPClass::getSingleton()

#define SAFE_DELETE(p)			{ if (p) { delete (p); (p) = NULL; } }
#define	SAFE_DELETE_ARRAY(p)	{ if (p) { delete[] (p); (p) = NULL;} }

/*
// new (new char)로 생성된 메모리 p를 제거 합니다.
#define SAFE_DELETE( p ) { if( p ) { delete ( p ); ( p ) = NULL; } }

// new (new char *) 로 생성된 배열 p를 제거 합니다.
#define SAFE_DELETE_ARRAY( p ) { if( p ) { delete[] ( p ); ( p ) = NULL; } }
*/