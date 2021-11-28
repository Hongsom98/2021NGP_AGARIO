#pragma once
#include "PacketDefine.h"
#pragma warning(disable : 4996)

WSADATA wsa;
SOCKET sock;
SOCKADDR_IN serveraddr;

#define SERVERIP "127.0.0.1"

void err_quit(const char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
    LocalFree(lpMsgBuf);
    exit(1);
}

void err_display(const char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    printf("[%s] %s", msg, (char*)lpMsgBuf);
    char* temp = (char*)lpMsgBuf;
    LocalFree(lpMsgBuf);
}

int recvn(SOCKET s, char* buf, int len, int flags)
{
    int received;
    char* ptr = buf;
    int left = len;

    while (left > 0)
    {
        received = recv(s, ptr, left, flags);
        if (received == SOCKET_ERROR)
            return SOCKET_ERROR;
        else if (received == 0)
            break;
        left -= received;
        ptr += received;
    }

    return (len - left);
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
	if (sock == INVALID_SOCKET)
		err_quit("socket()");

	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(TCPPORT);

	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("connect()");

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
    int retval = recvn(sock, (char*)&temp, sizeof(ClientLoginOKPacket), 0);
    if (retval == SOCKET_ERROR) {
        err_display("recv()");
        return false;
    }

    return temp.type == NICKNAME_USE ? true : false;
}

void RecvObjects()
{

}