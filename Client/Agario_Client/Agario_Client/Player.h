#pragma once
#include "UserDefine.h"

class Player
{
private:
	PlayerInfo InfoData;

public:
	Player();

	void Init(const char* InputID);
	void Update(const PlayerInfo& NewData);
	void Draw(HDC hdc);
	float GetRadius();
	POINT GetCenter();
	TCHAR* GetID();
};
