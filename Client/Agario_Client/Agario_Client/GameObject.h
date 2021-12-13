#pragma once
#include "UserDefine.h"

class GameObject
{
private:
	Feed Data[MAXFEED];
	Projectile projectiles[MAXPROJ];
	COLORREF Brushes[MAXFEED];
public:
	GameObject();

	void Update(const Feed* NewFeed, const Projectile* NewProjectile);
	void Draw(HDC hdc);
};
