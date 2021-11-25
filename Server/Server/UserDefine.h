#pragma once


#define WINDOW_WIDTH	800
#define WINDOW_HEIGHT	800

#define MAXFEED			300

#define MAP_WIDTH		2500
#define MAP_HEIGHT		2500


struct Feed
{
	POINT Center;
	float Radiuse;
};

struct PlayerInfo
{
	POINT Center;
	float Radius;
	char nickname[12]{ 0 };
	float Score;
	COLORREF Color;
};