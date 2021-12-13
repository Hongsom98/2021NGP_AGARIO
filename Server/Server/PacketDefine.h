#pragma once

#define NICKNAME_ADD	10
#define NICKNAME_USE	11
#define NICKNAME_UNUSE	12
#define MAXUSER			13

#define GAMEOBJECTLIST	110
#define INPUTDATA		111
#define GAMEOVER		112

#define TCPPORT			4000

struct ClientLoginPacket
{
	char type;
	char size;
	char ID[12];
};

struct ClientLoginOKPacket
{
	char type;
	char size;
};

struct GameObejctPacket
{
	char type;
	char size;
	PlayerInfo playerlist[3];
	Feed feedlist[MAXFEED];
};

struct PlayerInputPacket
{
	char type;
	char size;
	POINT mousePos;
	char keyState;
};

struct GameOverPacket
{
	char type;
	char size;
};


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
    LocalFree(lpMsgBuf);
}

int recvn(SOCKET s, char* buf, int len, int flags)
{
    int received;
    char* ptr = buf;
    int left = len;

    while (left > 0) {
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