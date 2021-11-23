#include "pch.h"
#include "Player.h"
#pragma warning(disable : 4996)



Player::Player()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> uid(0, 255);
	memset(InfoData.ID, 0, sizeof(InfoData.ID));
	InfoData.Color = RGB(uid(gen), uid(gen), uid(gen));
	InfoData.Score = 0;
	for (int i = 0; i < 4; ++i) {
		InfoData.SellData[i].Center = POINT{ 0,0 };
		InfoData.SellData[i].Radius = -1;
	}
}

void Player::Init(const char* InputID)
{
	std::uniform_real_distribution<> urdw(10, MAP_WIDTH - 10);
	std::uniform_real_distribution<> urdh(10, MAP_HEIGHT - 10);
	strncpy(InfoData.ID, InputID, 12);
	InfoData.SellData[0].Center.x = 500; //urdw(gen)
	InfoData.SellData[0].Center.y = 500; //urdh(gen)
	InfoData.SellData[0].Radius = 50;
}

void Player::Update(const PlayerInfo& NewData)
{
	InfoData.Score = NewData.Score;
	memcpy(InfoData.SellData, NewData.SellData, sizeof(InfoData.SellData));
}

void Player::Draw(HDC hdc)
{
	HBRUSH myBrush = (HBRUSH)CreateSolidBrush(InfoData.Color);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, myBrush);

	for (int i = 0; i < 4; ++i)
		if(InfoData.SellData[i].Radius > 0)
			Ellipse(hdc, InfoData.SellData[i].Center.x - InfoData.SellData[i].Radius,
				InfoData.SellData[i].Center.y - InfoData.SellData[i].Radius,
				InfoData.SellData[i].Center.x + InfoData.SellData[i].Radius,
				InfoData.SellData[i].Center.y + InfoData.SellData[i].Radius);
	
	SelectObject(hdc, oldBrush);
	DeleteObject(myBrush);
		
	HPEN hpen = (HPEN)CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
	HPEN oldpen = (HPEN)SelectObject(hdc, hpen);
	SetBkMode(hdc, TRANSPARENT);
	TextOut(hdc, InfoData.SellData[0].Center.x - 10, InfoData.SellData[0].Center.y - 10,
		InfoData.ID, lstrlen(InfoData.ID));
	SelectObject(hdc, oldpen);
	DeleteObject(hpen);
	
}

POINT Player::GetCenter()
{
	return InfoData.SellData[0].Center;
}

TCHAR* Player::GetID()
{
	return (TCHAR*)InfoData.ID;
}