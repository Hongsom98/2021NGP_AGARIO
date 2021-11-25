#include "framework.h"
#include "UserDefine.h"
#include "PacketDefine.h"

using namespace std;
PlayerInfo Player[3];
USHORT ClientPorts[3];
int nowID = 0;

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
    cout << "중복 없음" << endl << endl;
    return true;
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

DWORD WINAPI ProcessClient(LPVOID arg)
{
    SOCKET client_sock = (SOCKET)arg;
    SOCKADDR_IN clientaddr;
    int retval;
    int addrlen;
    int ClientNum = nowID;

    addrlen = sizeof(clientaddr);
    getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);
    char type;
  
    if (Player[ClientNum].nickname[0] == '\0')
    {
        retval = recvn(client_sock, (char*)&type, sizeof(type), 0);

        switch (type)
        {
            case NICKNAME_ADD:
                ClientLoginPacket packet;
                retval = recvn(client_sock, (char*)&packet, sizeof(packet), 0);
                if (CheckID(packet.ID) && nowID < 3)
                {
                    SaveID(packet.ID);
                    SendID_OK(true, client_sock);
                    break;
                }
                else SendID_OK(false, client_sock); 
                break;
            default:
                break;
        }
    }
    
    closesocket(client_sock);

    return 0;
}

int main()
{
    int retval;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) err_quit("socket()");

    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(TCPPORT);
    retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("bind()");

    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");

    SOCKET client_sock;
    SOCKADDR_IN clientaddr;
    int addrlen;

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

        hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
        if (hThread == NULL) { closesocket(client_sock); }
        else { CloseHandle(hThread); }
    }
    
    closesocket(listen_sock);

    WSACleanup();
}