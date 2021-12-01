#include "framework.h"
#include "UserDefine.h"
#include "PacketDefine.h"
#include "Timer.h"

using namespace std;
PlayerInfo Player[3];
Feed feed[MAXFEED];
USHORT ClientPorts[3];
int nowID = 0;
HANDLE ClientEvent[3];
HANDLE UpdateEvent;
queue<Input> InputQueue;
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> urdw(10, WINDOW_WIDTH - 10);
std::uniform_real_distribution<> urdh(10, WINDOW_HEIGHT - 10);
std::uniform_int_distribution<> uidc(0, 255);

void InitPlayers()
{
    Player[0].SellData[0].Center.x = 300;
    Player[0].SellData[0].Center.y = 300;
    Player[0].SellData[0].Radius = 10;
    Player[0].Color = RGB(uidc(gen), uidc(gen), uidc(gen));
    Player[1].SellData[0].Center.x = 400;
    Player[1].SellData[0].Center.y = 400;
    Player[1].SellData[0].Radius = 10;
    Player[1].Color = RGB(uidc(gen), uidc(gen), uidc(gen));
    Player[2].SellData[0].Center.x = 200;
    Player[2].SellData[0].Center.y = 200;
    Player[2].SellData[0].Radius = 10;
    Player[2].Color = RGB(uidc(gen), uidc(gen), uidc(gen));
}

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
            cout << "중복있음" << endl;
            return false;
        }
    }
    cout << "중복 없음" << endl;
    return true;
}

void PlayerMove(const Input& input)
{
    if (!input.ClientNum) cout << input.mousePos.x << ", " << input.mousePos.y << endl;
    float xVec = (input.mousePos.x - WINDOW_WIDTH / 2) - Player[input.ClientNum].SellData[0].Center.x;
    float yVec = (input.mousePos.y - WINDOW_HEIGHT / 2) - Player[input.ClientNum].SellData[0].Center.y;
    float Distance = sqrtf(powf(xVec, 2) + powf(yVec, 2));
    xVec /= Distance;
    yVec /= Distance;
    xVec = round(xVec);
    yVec = round(yVec);
    if (!input.ClientNum) cout << input.ClientNum << " : " << xVec << "  " << yVec << endl;
    
    for (int i = 0; i < 4; ++i) {
        
        if (Player[input.ClientNum].SellData[i].Radius) {
            
            Player[input.ClientNum].SellData[i].Center.x += xVec * 1.0f;
            Player[input.ClientNum].SellData[i].Center.y += yVec * 1.0f;
        }
    }
}

void SendObjectList(SOCKET client_sock)
{
    int retval;
    GameObejctPacket temp;
    temp.type = GAMEOBJECTLIST;
    temp.size = sizeof(temp);
    memcpy(temp.playerlist, Player, sizeof(PlayerInfo) * 3);
    memcpy(temp.feedlist, feed, sizeof(Feed) * MAXFEED);

    retval = send(client_sock, (char*)&temp, sizeof(temp), 0);
    if (retval == SOCKET_ERROR) err_display("Client Thread gobj send()");
}

//BOOL isColidePlayerToPlayer(PlayerInfo Client, int ClientNum)
//{
//    //switch (ClientNum)
//    //{
//    //case 1:
//    //    if (sqrt(pow(Client.Center.x - Player[1].Center.x, 2) + pow(Client.Center.y - Player[1].Center.y, 2)) < Client.Radius + Player[1].Radius)
//    //    {
//
//    //    }
//    //    if (sqrt(pow(Client.Center.x - Player[2].Center.x, 2) + pow(Client.Center.y - Player[2].Center.y, 2)) < Client.Radius + Player[2].Radius) return true;
//    //    else return false;
//    //    break;
//    //case 2:
//    //    break;
//    //case 3:
//    //    break;
//    //default:
//    //    break;
//    //}
//}

BOOL isColidePlayerToFeed(PlayerInfo Client)
{
    for (int i = 0; i < MAXFEED; ++i)
    {
        for (int j = 0; i < 4; ++j) {
            if (sqrt(pow(Client.SellData[j].Center.x - feed[i].Center.x, 2) + pow(Client.SellData[j].Center.y - feed[i].Center.y, 2)) < Client.SellData[j].Radius + feed[i].Radius)
            {
                Client.SellData[j].Radius += feed[i].Radius;
                feed[i].Center.x = urdw(gen);
                feed[i].Center.y = urdh(gen);

                return true;
            }
        }
    }

    return false;
}

DWORD WINAPI ProcessClient(LPVOID arg)
{
    SOCKET client_sock = (SOCKET)arg;
    int retval;
    int ClientNum = nowID;
    char type;

    while (true) {
        WaitForSingleObject(ClientEvent[ClientNum], INFINITE);

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
            {
                PlayerInputPacket packet;
                retval = recvn(client_sock, (char*)&packet, sizeof(packet), 0);
                if (retval == SOCKET_ERROR) err_display("Client Thread Input recv()");
                InputQueue.push({ ClientNum, packet.keyState, packet.mousePos });
            }
                break;
        }

        if (ClientNum < 2) SetEvent(ClientEvent[ClientNum + 1]);
        if (ClientNum == 2) SetEvent(UpdateEvent);

        WaitForSingleObject(ClientEvent[ClientNum], INFINITE);
        /*GameObejctPacket temp;
        temp.size = sizeof(GameObejctPacket); temp.type = GAMEOBJECTLIST;
        memcpy(temp.playerlist, Player, sizeof(PlayerInfo) * 3);
        memcpy(temp.feedlist, feed, sizeof(Feed) * MAXFEED);
        retval = send(client_sock, (char*)&temp, sizeof(temp), 0);
        if (retval == SOCKET_ERROR) err_display("Client Thread gobj send()");*/
        SendObjectList(client_sock);

        SetEvent(ClientEvent[(ClientNum + 1) % 3]);
    }

    closesocket(client_sock);
    return 0;
}

DWORD WINAPI ProcessUpdate(LPVOID arg)
{
    while (true)
    {
        WaitForSingleObject(UpdateEvent, INFINITE);
        while (!InputQueue.empty())
        {
            Input temp = InputQueue.front();
            InputQueue.pop();

            PlayerMove(temp);
        }
        SetEvent(ClientEvent[0]);
    }

    return 0;
}

int main()
{
    for (int i = 0; i < MAXFEED; ++i)
    {
        feed[i].Center.x = urdw(gen);
        feed[i].Center.y = urdh(gen);
        feed[i].Radius = 3;
    }
    InitPlayers();
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

    ClientEvent[0] = CreateEvent(NULL, FALSE, TRUE, NULL);
    ClientEvent[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
    ClientEvent[2] = CreateEvent(NULL, FALSE, FALSE, NULL);
    UpdateEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

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