#include "pch.h"
#include "Player.h"
#pragma warning(disable : 4996)

Player::Player()
{

}

void Player::Init(const char* InputID)
{

}

void Player::Update(const PlayerInfo& NewData)
{
	InfoData.Color = NewData.Color;
	InfoData.Score = NewData.Score;
	strcpy(InfoData.ID, NewData.ID);
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