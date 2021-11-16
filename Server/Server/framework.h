#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#define WINDOW_WIDTH	960
#define WINDOW_HEIGHT	540

#define TCPPORT			54321

#define MAXFEED			300
#define MAXTRAP			30
#define MAXBOT			10

#include "PacketDefine.h"



struct PlayerInfo
{
	POINT Center;
	float Radius;
	char nickname[12];
	float Score;
	COLORREF Color;
};