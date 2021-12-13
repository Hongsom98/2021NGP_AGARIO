#include "framework.h"
#include "UserDefine.h"
#include "PacketDefine.h"
#include "Timer.h"

using namespace std;
PlayerInfo Player[3];
Feed feed[MAXFEED];
Projectile projectiles[20];
USHORT ClientPorts[3];
int nowID = 0;
HANDLE ClientEvent[3];
HANDLE UpdateEvent;
queue<Input> InputQueue;
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> urdw(10, 784);
std::uniform_real_distribution<> urdh(10, 761);
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
        retval = send(client_sock, (char*)&packet.type, sizeof(packet.type), 0);
        retval = send(client_sock, (char*)&packet, sizeof(packet),0);
    }
    else {
        packet.type = NICKNAME_UNUSE;
        retval = send(client_sock, (char*)&packet.type, sizeof(packet.type), 0);
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
    float xVec = input.mousePos.x - Player[input.ClientNum].SellData[0].Center.x;
    float yVec = input.mousePos.y - Player[input.ClientNum].SellData[0].Center.y;
    float Distance = sqrtf(powf(xVec, 2) + powf(yVec, 2));
    xVec /= Distance;
    yVec /= Distance;

    for (int i = 0; i < 4; ++i) {
        if (Player[input.ClientNum].SellData[i].Radius > 0) {
            Player[input.ClientNum].SellData[i].Center.x += xVec * 2.0f;
            if (Player[input.ClientNum].SellData[i].Center.x < 0)  Player[input.ClientNum].SellData[i].Center.x = 0;
            if (Player[input.ClientNum].SellData[i].Center.x > 784) Player[input.ClientNum].SellData[i].Center.x = 784;
            Player[input.ClientNum].SellData[i].Center.y += yVec * 2.0f;
            if (Player[input.ClientNum].SellData[i].Center.y < 0) Player[input.ClientNum].SellData[i].Center.y = 0;
            if (Player[input.ClientNum].SellData[i].Center.y > 761) Player[input.ClientNum].SellData[i].Center.y = 761;
        }
    }

}

void ProjectileMove()
{
    for (int i = 0; i < MAXPROJ; ++i) {
        if (projectiles[i].Color) {
            projectiles[i].Center.x += projectiles[i].xSpeed * 10.0f;
            projectiles[i].Center.y += projectiles[i].ySpeed * 10.0f;
            if (projectiles[i].xSpeed > 0.01) projectiles[i].xSpeed /= 4.f;
            else projectiles[i].xSpeed = 0;
            if (projectiles[i].ySpeed > 0.01) projectiles[i].ySpeed /= 4.f;
            else projectiles[i].ySpeed = 0;
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
    memcpy(temp.projectile, projectiles, sizeof(Projectile) * MAXPROJ);

    retval = send(client_sock, (char*)&temp.type, sizeof(temp.type), 0);
    retval = send(client_sock, (char*)&temp, sizeof(temp), 0);
    if (retval == SOCKET_ERROR) err_display("Client Thread gobj send()");
}

void isColidePlayerToPlayer(PlayerInfo& Client, int ClientNum)
{
    switch (ClientNum)
    {
    case 0:
        if (sqrt(pow(Client.SellData[0].Center.x - Player[1].SellData[0].Center.x, 2) +
            pow(Client.SellData[0].Center.y - Player[1].SellData[0].Center.y, 2))
            < Client.SellData[0].Radius + Player[1].SellData[0].Radius)
        {
            if (Client.SellData[0].Radius < Player[1].SellData[0].Radius)
            {
                Player[1].SellData[0].Radius += Client.SellData[0].Radius;
                Client.SellData[0].Radius = 0;
                break;
            }
            else
            {
                Client.SellData[0].Radius += Player[1].SellData[0].Radius;
                Player[1].SellData[0].Radius = 0;
                break;
            }
        }
        if (sqrt(pow(Client.SellData[0].Center.x - Player[2].SellData[0].Center.x, 2) +
            pow(Client.SellData[0].Center.y - Player[2].SellData[0].Center.y, 2))
            < Client.SellData[0].Radius + Player[2].SellData[0].Radius)
        {
            if (Client.SellData[0].Radius < Player[2].SellData[0].Radius)
            {
                Player[2].SellData[0].Radius += Client.SellData[0].Radius;
                Client.SellData[0].Radius = 0;
                break;
            }
            else
            {
                Client.SellData[0].Radius += Player[2].SellData[0].Radius;
                Player[2].SellData[0].Radius = 0;
                break;
            }
        }
        break;
    case 1:
        if (sqrt(pow(Client.SellData[0].Center.x - Player[0].SellData[0].Center.x, 2) +
            pow(Client.SellData[0].Center.y - Player[0].SellData[0].Center.y, 2))
            < Client.SellData[0].Radius + Player[0].SellData[0].Radius)
        {
            if (Client.SellData[0].Radius < Player[0].SellData[0].Radius)
            {
                Player[0].SellData[0].Radius += Client.SellData[0].Radius;
                Client.SellData[0].Radius = 0;
                break;
            }
            else
            {
                Client.SellData[0].Radius += Player[0].SellData[0].Radius;
                Player[0].SellData[0].Radius = 0;
                break;
            }
        }
        if (sqrt(pow(Client.SellData[0].Center.x - Player[2].SellData[0].Center.x, 2) +
            pow(Client.SellData[0].Center.y - Player[2].SellData[0].Center.y, 2))
            < Client.SellData[0].Radius + Player[2].SellData[0].Radius)
        {
            if (Client.SellData[0].Radius < Player[2].SellData[0].Radius)
            {
                Player[2].SellData[0].Radius += Client.SellData[0].Radius;
                Client.SellData[0].Radius = 0;
                break;
            }
            else
            {
                Client.SellData[0].Radius += Player[2].SellData[0].Radius;
                Player[2].SellData[0].Radius = 0;
                break;
            }
        }
        break;
    case 2:
        if (sqrt(pow(Client.SellData[0].Center.x - Player[0].SellData[0].Center.x, 2) +
            pow(Client.SellData[0].Center.y - Player[0].SellData[0].Center.y, 2))
            < Client.SellData[0].Radius + Player[0].SellData[0].Radius)
        {
            if (Client.SellData[0].Radius < Player[0].SellData[0].Radius)
            {
                Player[0].SellData[0].Radius += Client.SellData[0].Radius;
                Client.SellData[0].Radius = 0;
                break;
            }
            else
            {
                Client.SellData[0].Radius += Player[0].SellData[0].Radius;
                Player[0].SellData[0].Radius = 0;
                break;
            }
        }
        if (sqrt(pow(Client.SellData[0].Center.x - Player[1].SellData[0].Center.x, 2) +
            pow(Client.SellData[0].Center.y - Player[1].SellData[0].Center.y, 2))
            < Client.SellData[0].Radius + Player[1].SellData[0].Radius)
        {
            if (Client.SellData[0].Radius < Player[1].SellData[0].Radius)
            {
                Player[1].SellData[0].Radius += Client.SellData[0].Radius;
                Client.SellData[0].Radius = 0;
                break;
            }
            else
            {
                Client.SellData[0].Radius += Player[1].SellData[0].Radius;
                Player[1].SellData[0].Radius = 0;
                break;
            }
        }
        break;
    default:
        break;
    }
}

void isColidePlayerToFeed(PlayerInfo& Client)
{
    for (int i = 0; i < MAXFEED; ++i)
    {
        for (int j = 0; j < 4; ++j) {
            if (sqrt(pow(Client.SellData[j].Center.x - feed[i].Center.x, 2) + pow(Client.SellData[j].Center.y - feed[i].Center.y, 2)) < Client.SellData[j].Radius + feed[i].Radius)
            {
                Client.SellData[j].Radius += 0.3;
                Client.Score += 1;
                feed[i].Center.x = urdw(gen);
                feed[i].Center.y = urdh(gen);
            }
        }
    }
}

void ColidePlayerToProjectile()
{
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < MAXPROJ; ++j) {
            if (projectiles[j].Color) {
                for (int k = 0; k < 4; ++i) {
                    if (Player[i].SellData[k].Radius) {
                        if (sqrt(pow(Player[i].SellData[k].Center.x - projectiles[j].Center.x, 2) + pow(Player[i].SellData[k].Center.y - projectiles[j].Center.y, 2)) < Player[i].SellData[k].Radius + 6) {
                            if (Player[i].Color != projectiles[j].Color) Player[i].Score++;
                            Player[i].SellData[k].Radius += 0.6;
                            projectiles[j].Color = 0;
                            break;
                        }
                    }
                }
            }
        }
    }
}

void SpitFeed(const Input& input)
{
    for (int i = 0; i < 4; ++i) {
        if (Player[input.ClientNum].SellData[i].Radius > 0) {
            for (int j = 0; j < MAXPROJ; ++j) {
                if (!projectiles[j].Color) {
                    float xVec = input.mousePos.x - Player[input.ClientNum].SellData[i].Center.x;
                    float yVec = input.mousePos.y - Player[input.ClientNum].SellData[i].Center.y;
                    float Distance = sqrtf(powf(xVec, 2) + powf(yVec, 2));
                    xVec /= Distance;
                    yVec /= Distance;
                    projectiles[j].Center.x = Player[input.ClientNum].SellData[i].Center.x + Player[input.ClientNum].SellData[i].Radius + xVec * 8.f;
                    projectiles[j].Center.y = Player[input.ClientNum].SellData[i].Center.y + Player[input.ClientNum].SellData[i].Radius + yVec * 8.f;
                    projectiles[j].Color = Player[input.ClientNum].Color;
                    projectiles[j].xSpeed = xVec;
                    projectiles[j].ySpeed = yVec;
                    Player[input.ClientNum].SellData[i].Radius -= 6;
                    return;
                }
            }
        }
    }
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
            ProjectileMove();
            if (temp.InputKey == 'x') SpitFeed(temp);
            isColidePlayerToFeed(Player[temp.ClientNum]);
            isColidePlayerToPlayer(Player[temp.ClientNum], temp.ClientNum);
            ColidePlayerToProjectile();
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