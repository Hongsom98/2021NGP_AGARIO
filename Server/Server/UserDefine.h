#pragma once


#define WINDOW_WIDTH	800
#define WINDOW_HEIGHT	800
#define MAXFEED			300
#define MAXPROJ			50

struct Feed
{
	POINT Center;
	float Radius = 0;
};

struct Projectile
{
	POINT Center{ 0,0 };
	float Radius = 6;
	float xSpeed;
	float ySpeed;
	COLORREF Color=0;
	float MoveDist = 0;
};

struct CObject
{
	POINT Center;
	float Radius = 0;
};

struct PlayerInfo
{
	CObject SellData[4];
	char ID[12];
	float Score = 10;
	COLORREF Color;
};

struct Input
{
	int ClientNum;
	char InputKey;
	POINT mousePos;
};