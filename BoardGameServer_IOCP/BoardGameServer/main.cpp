#include <locale.h>//한국어 깨짐 방지
#include <stdio.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <process.h>
#include <iostream>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib")
#define _CRT_SECURE_NO_WARNINGS    // strcat 보안 경고로 인한 컴파일 에러 방지
#pragma warning(disable:4996)

using namespace std;

#include "ChannelManager.h"

//=====================================================

//메인문

//Server Init부분

//메인 문에서 정리함
//서버에서 해야할 것

// 가장 중요 - 채널 및 채널 매니저 구현

//1. 룸 게임 중인지 여부
//2. 룸 리스트와 로비 유저 리스트 전송 방식 테스트 및 확실히 구현
//			(일단 새로고침할 경우에만, 이후 로비 유저 리스트 정도는 자동으로 전송)
//3. 인게임 제한 시간
//4. 방장 (필요한가? 아직 보류, 일단 적어둠)
//5. 방 제목 구현

//클라에서 해야할 것
//1. 방 번호 방 내부에서 표기하기.
//2. 클라 좀 이쁘게(연출이나 그래픽 리소스)

//개인적으로 생각하는 현재 문제점
//20191006
//	- SelectClass, Lobby, RoomManager 상호 참조가 너무 많음.
//	- Thread 이해가 아직도 부족함. 더미 클라도 그렇고 채널도 그렇고 쓰레드 여러개 어떻게 돌려야하지?
//			20191014
//				대충 돌아가게는 함. 확실하게 잘 되는지는 모르겠다. 테스트 더 해봐야

//=====================================================
//임시
//동기화 부분
// - 초당 7번의 패킷 전송 기준으로 3천명이 도는게 빠른지 있는것만 큐에만 있는 걸로 도는 것이 빠른지 테스트
//테스트 환경은 플레이 환경으로 생각해야
//=====================================================

int main()
{
	_wsetlocale(LC_ALL, L"korean");      //지역화 설정을 전역적으로 적용
	wcout.imbue(locale("korean"));       //출력시 부분적 적용
	wcin.imbue(locale("korean"));        //입력시 부분적 적용

	//======================================================

	ChannelManager channelManager;
	channelManager.Init();

	while (1)
	{

	}

	WSACleanup();

	return 0;
}