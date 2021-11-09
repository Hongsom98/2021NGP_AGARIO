#pragma once

#define NICKNAME_ADD	10
#define NICKNAME_USE	11
#define NICKNAME_UNUSE	12
#define MAXUSER			13

#define PLAYERLIST		110
#define GAMEOBJECTLIST	111
#define BOTLIST			112
#define GAMEOVER		113

struct Feed
{
	float Center;
	float Radiuse;
};
struct Trap
{
	float Center;
	float Radius;
};
struct GameObject
{
	float Center;
	float Radius;
};
struct BOT
{

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

struct PlayerListPacket
{
	char size;
	char type;
	PlayerInfo playerlist[3];
	char rank[3];
};

struct GameObjectListPacket
{
	char size;
	char type;
	Feed list[MAXFEED];
	Trap list[MAXTRAP];
};

struct BOTLIST
{
	char size;
	char type;
	BOT botlist[MAXBOT];
};

struct GameOverPacket
{
	char size;
	char type;
};