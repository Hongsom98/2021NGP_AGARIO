#pragma once

#define WINDOW_WIDTH	800
#define WINDOW_HEIGHT	800

#define MAXFEED			300

struct Feed
{
	POINT Center;
	float Radius=0;
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