#pragma once


#define WINDOW_WIDTH	800
#define WINDOW_HEIGHT	800

#define TCPPORT			54321

#define MAXFEED			300

#define MAP_WIDTH		2560
#define MAP_HEIGHT		1440

struct Feed
{
	POINT Center;
	float Radius;
};

struct CObject
{
	POINT Center;
	float Radius;
};

struct PlayerInfo
{
	CObject SellData[4];
	char ID[12];
	float Score;
	COLORREF Color;
};