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

//#define SERVERIP "112.152.55.39"
#define SERVERIP "127.0.0.1"

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

void SendCanUseNickName(char* ID)
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

GameObjectPacket RecvObjects()
{
    GameObjectPacket temp;
    recvn(sock, (char*)&temp.type, sizeof(temp.type), 0);

    int retval = recvn(sock, (char*)&temp, sizeof(GameObjectPacket), 0);
    if (retval == SOCKET_ERROR) {
        err_display("recv()");
    }
    return temp;
}