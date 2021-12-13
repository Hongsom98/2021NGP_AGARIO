#pragma once
#include "..\..\..\Server\Server\PacketDefine.h"
#include "Player.h"
#include "GameObject.h"
#include <mutex>
#pragma warning(disable : 4996)

Player player[3];
GameObject feeds;
WSADATA wsa;
SOCKET sock;
SOCKADDR_IN serveraddr;
bool isConnection{ false };
enum USENICKNAME { NICK_YET, NICK_OK, NICK_NON };
USENICKNAME nick = NICK_YET;
#define SERVERIP "112.152.55.39"
//#define SERVERIP "127.0.0.1"

DWORD WINAPI RecvThread(LPVOID arg)
{
    int retval;
    char type;
    

    while (true)
    {
        retval = recvn(sock, (char*)&type, sizeof(type), 0);
        if (retval == SOCKET_ERROR) {
            err_quit("RecvThread recv()");
            break;
        }
        
        switch (type)
        {

        case NICKNAME_USE: {
            nick = NICK_OK;
            break;
        }
        case NICKNAME_UNUSE: {
            nick = NICK_NON;
            break;
        }
        case GAMEOBJECTLIST: {
            GameObejctPacket packet;
            recvn(sock, (char*)&packet, sizeof(packet), 0);
            for (int i = 0; i < CLIENT; ++i) player[i].Update(packet.playerlist[i]);
            feeds.Update(packet.feedlist, packet.projectile);
            break;
        }

        }
        
    }
    closesocket(sock);
    return 0;
}

void SendInputData(POINT p, char Key = 'N')
{
    PlayerInputPacket temp;
    temp.size = sizeof(PlayerInputPacket);
    temp.type = INPUTDATA;
    temp.mousePos = p;
    temp.keyState = Key;
    
    send(sock, (char*)&temp.type, sizeof(temp.type), 0);
    send(sock, (char*)&temp, sizeof(temp), 0);
}

void SendID(char* ID)
{
	int retval;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		exit(1);

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(TCPPORT);

	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

    ClientLoginPacket temp;
    temp.size = sizeof(ClientLoginPacket);
    temp.type = NICKNAME_ADD;
    strncpy(temp.ID, ID, strlen(ID));
    temp.ID[strlen(ID)] = '\0';

    send(sock, (char*)&temp.type, sizeof(temp.type), 0);
    send(sock, (char*)&temp, sizeof(temp), 0);
}

bool RecvIDCheck()
{

    ClientLoginOKPacket temp;
    recvn(sock, (char*)&temp.type, sizeof(temp.type), 0);
    int retval = recvn(sock, (char*)&temp, sizeof(ClientLoginOKPacket), 0);
    if (retval == SOCKET_ERROR) {
        err_display("recv()");
        return false;
    }

    return temp.type == NICKNAME_USE ? true : false;
}

GameObejctPacket RecvObjects()
{
    GameObejctPacket temp;

    int retval = recvn(sock, (char*)&temp, sizeof(GameObejctPacket), 0);
    if (retval == SOCKET_ERROR) {
        err_display("recv()");
    }
    return temp;
}