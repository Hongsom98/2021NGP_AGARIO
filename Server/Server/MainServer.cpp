#include "framework.h"
#include "UserDefine.h"
#include "PacketDefine.h"
#include "Timer.h"

using namespace std;
PlayerInfo Player[3];
USHORT ClientPorts[3];
Feed feedlist[MAXFEED];
int nowID = 0;
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> urdw(10, MAP_WIDTH - 10);
std::uniform_real_distribution<> urdh(10, MAP_HEIGHT - 10);

void SaveID(const char* NewID)
{
    strncpy(Player[nowID].ID, NewID, strlen(NewID));
    nowID++;

    cout << "저장완료" << endl;
}

void SendID_OK(bool duplicated, SOCKET client_sock, const char* NewID) {
    ClientLoginOKPacket packet;
    packet.size = sizeof(packet);
    int retval;
    if (duplicated) {
        SaveID(NewID);
        packet.type = NICKNAME_USE;
        retval = send(client_sock, (char*)&packet, sizeof(packet),0);
    }
    else {
        packet.type = NICKNAME_UNUSE;
        retval = send(client_sock, (char*)&packet, sizeof(packet), 0);
    }
    if (retval == SOCKET_ERROR) err_display("Client Thread ID send()");
}

bool CheckID(const char* ID)
{
    for (int i = 0; i < nowID; ++i)
    {
        if (strcmp(Player[i].ID, ID) == false) {
            cout << "중복있음" << endl << endl;
            return false;
        }
    }
    cout << "중복 없음" << endl << endl;
    return true;
}

void SendObjectList(SOCKET client_sock)
{
    GameObejctPacket temp;
    temp.type = GAMEOBJECTLIST;
    temp.size = sizeof(temp);

    for (int i = 0; i < 3; ++i)
    {
        temp.playerlist[i] = Player[i];
    }

    for (int i = 0; i < MAXFEED; ++i)
    {
        temp.feedlist[i] = feedlist[i];
    }

    send(client_sock, (char*)&temp, sizeof(temp), 0);
}

BOOL isColidePlayerToPlayer(PlayerInfo Client, int ClientNum)
{
    //switch (ClientNum)
    //{
    //case 1:
    //    if (sqrt(pow(Client.Center.x - Player[1].Center.x, 2) + pow(Client.Center.y - Player[1].Center.y, 2)) < Client.Radius + Player[1].Radius)
    //    {

    //    }
    //    if (sqrt(pow(Client.Center.x - Player[2].Center.x, 2) + pow(Client.Center.y - Player[2].Center.y, 2)) < Client.Radius + Player[2].Radius) return true;
    //    else return false;
    //    break;
    //case 2:
    //    break;
    //case 3:
    //    break;
    //default:
    //    break;
    //}
}

BOOL isColidePlayerToFeed(PlayerInfo Client)
{
    for (int i = 0; i < MAXFEED; ++i)
    {
        if (sqrt(pow(Client.Center.x - feedlist[i].Center.x, 2) + pow(Client.Center.y - feedlist[i].Center.y, 2)) < Client.Radius + feedlist[i].Radiuse)
        {
            Client.Radius += feedlist[i].Radiuse;
            feedlist[i].Center.x = urdw(gen);
            feedlist[i].Center.y = urdh(gen);

            return true;
        }
    }
}

DWORD WINAPI ProcessClient(LPVOID arg)
{
    SOCKET client_sock = (SOCKET)arg;
    int retval;
    char type;

    while (true) {
        retval = recvn(client_sock, (char*)&type, sizeof(type), 0);
        if (retval == SOCKET_ERROR) err_display("Client Thread Type recv()");

        switch (type) {
            case NICKNAME_ADD:
            {
                ClientLoginPacket packet;
                retval = recvn(client_sock, (char*)&packet, sizeof(packet), 0);
                if (retval == SOCKET_ERROR) err_display("Client Thread ID recv()");

                if (CheckID(packet.ID) && nowID < 3) SendID_OK(true, client_sock, packet.ID);
                else SendID_OK(false, client_sock, NULL);
            }
                break;
            case INPUTDATA:
                break;
        }
    }

    closesocket(client_sock);
    return 0;
}

DWORD WINAPI ProcessUpdate(LPVOID arg)
{

    return 0;
}

int main()
{
    for (int i = 0; i < MAXFEED; ++i)
    {
        feedlist[i].Center.x = urdw(gen);
        feedlist[i].Center.y = urdh(gen);
        feedlist[i].Radiuse = 10;
    }
    int retval;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) err_quit("listen socket()");

    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(TCPPORT);
    retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("listen bind()");

    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");

    SOCKET client_sock;
    SOCKADDR_IN clientaddr;
    int addrlen;

    HANDLE hThread;
    hThread = CreateThread(NULL, 0, ProcessUpdate, NULL, 0, NULL);
    if (hThread != NULL) { CloseHandle(hThread); cout << "업데이트 쓰레드 생성" << endl; }

    while (1) {
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            err_display("accept()");
            break;
        }

        printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
        if (hThread == NULL) { closesocket(client_sock); }
        else { CloseHandle(hThread); }
    }
    
    closesocket(listen_sock);
    WSACleanup();
}