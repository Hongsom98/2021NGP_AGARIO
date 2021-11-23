#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <iostream>
#include <WinSock2.h>
#include "PacketDefine.h"
//#include "framework.h"
#pragma warning(disable:4996)

using namespace std;
PlayerInfo Player[3];
int nowID{};

void SaveID(const char* NewID)
{
    strncpy(Player[nowID].nickname, NewID, strlen(NewID));
    nowID++;

    cout << "저장완료" << endl;
}
void SendID_OK(bool duplicated, SOCKET client_sock) {
    if (duplicated)
    {
        ClientLoginOKPacket packet;
        packet.size = sizeof(packet);
        packet.type = NICKNAME_USE;
        int retval = send(client_sock, (char*)&packet, sizeof(packet),0);
    }
    else
    {
        ClientLoginOKPacket packet;
        packet.size = sizeof(packet);
        packet.type = NICKNAME_UNUSE;
        int retval = send(client_sock, (char*)&packet, sizeof(packet), 0);
    }
}
bool CheckID(const char* ID)
{
    for (int i = 0; i < nowID; ++i)
    {
        if (strcmp(Player[i].nickname, ID) == false) {
            cout << "중복있음" << endl << endl;
            return false;
        }
    }
    SaveID(ID);
    cout << "중복 없음" << endl << endl;
    return true;
}
void SendPlayerList(SOCKET client_sock)
{
    PlayerListPacket packet;
    packet.type = PLAYERLIST;
    
    packet.size = sizeof(packet);
    
    for (int i = 0; i < nowID; ++i)
    {
        packet.Playerlists[i] = Player[i];
        packet.rank[i] = Player[i].Score;
    }
    int retval = send(client_sock, (char*)&packet, sizeof(packet), 0);
    
}
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

// 소켓 함수 오류 출력
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

// 클라이언트와 데이터 통신
DWORD WINAPI ProcessClient(LPVOID arg)
{
    SOCKET client_sock = (SOCKET)arg;
    int retval;
    SOCKADDR_IN clientaddr;
    int addrlen;

    // 클라이언트 정보 얻기
    addrlen = sizeof(clientaddr);
    getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);
    char type;

    while (1) {
        while (1) {
            retval = recvn(client_sock, (char*)&type, sizeof(type), 0);

            switch (type)
            {
            case NICKNAME_ADD:
                ClientLoginPacket packet;
                retval = recvn(client_sock, (char*)&packet, sizeof(packet), 0);
                if (CheckID(packet.nickname) && nowID < 3)
                {
                    SaveID(packet.nickname);
                    SendID_OK(true, client_sock);
                    break;
                }
                else SendID_OK(false, client_sock); break;

            default:
                break;
            }
        }
    }
    closesocket(client_sock);

    return 0;
}

int main()
{
    int retval;

    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // socket()
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) err_quit("socket()");

    // bind()
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(TCPPORT);
    retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("bind()");

    // listen()
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");

    // 데이터 통신에 사용할 변수
    SOCKET client_sock;
    SOCKADDR_IN clientaddr;
    int addrlen;
    char ID[256] = "";
    int len;

    char type;

    HANDLE hThread;
    
    while (1) {

        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            err_display((char*)"accept()");
            break;
        }

        printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
            inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        hThread = CreateThread(NULL, 0, ProcessClient,
            (LPVOID)client_sock, 0, NULL);
        if (hThread == NULL) { closesocket(client_sock); }
        else { CloseHandle(hThread); }
    }
    
    closesocket(listen_sock);

    WSACleanup();
    return 0;
}