#pragma once
#include "UserDefine.h"

class GameObject
{
private:
	Feed Data[MAXFEED];
	COLORREF Brushes[MAXFEED];
public:
	GameObject();

	void Update(const Feed* NewFeed);
	void Draw(HDC hdc);
};
