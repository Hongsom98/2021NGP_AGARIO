#pragma once

#define NICKNAME_ADD	10
#define NICKNAME_USE	11
#define NICKNAME_UNUSE	12
#define MAXUSER			13

#define GAMEOBJECTLIST	110
#define INPUTDATA		111
#define GAMEOVER		112

struct Feed
{
	float Center;
	float Radiuse;
};

struct ClientLoginPacket
{
	char size;
	char type;
	char nickname[12];
};

struct ClientLoginOKPacket
{
	char size;
	char type;
};

struct GameObejctPacket
{
	char size;
	char type;
	PlayerInfo playerlist[3];
	Feed feedlist[MAXFEED];
};

struct PlayerInputPacket
{
	char size;
	char type;
	POINT mousePos;
	char keyState;
};

struct GameOverPacket
{
	char size;
	char type;
};