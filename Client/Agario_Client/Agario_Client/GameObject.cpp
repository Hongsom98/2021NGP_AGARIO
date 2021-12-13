#include "pch.h"
#include "GameObject.h"



GameObject::GameObject()
{
	std::random_device frd;
	std::mt19937 fgen(frd());
	std::uniform_int_distribution<> fuid(0, 255);
	for (int i = 0; i < MAXFEED; ++i)
		Brushes[i] = RGB(fuid(fgen), fuid(fgen), fuid(fgen));
}

void GameObject::Update(const Feed* NewFeed, const Projectile* NewProjectile)
{
	memcpy(Data, NewFeed, sizeof(Feed) * MAXFEED);
	memcpy(projectiles, NewProjectile, sizeof(Projectile) * MAXPROJ);
}

void GameObject::Draw(HDC hdc)
{
	for (int i = 0; i < MAXPROJ; ++i)
	{
		if (projectiles[i].Color) {
			HBRUSH myBrush = (HBRUSH)CreateSolidBrush(projectiles[i].Color);
			HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, myBrush);
			Ellipse(hdc, projectiles[i].Center.x - projectiles[i].Radius,
				projectiles[i].Center.y - projectiles[i].Radius,
				projectiles[i].Center.x + projectiles[i].Radius,
				projectiles[i].Center.y + projectiles[i].Radius);

			SelectObject(hdc, oldBrush);
			DeleteObject(myBrush);
		}
	}

	for (int i = 0; i < MAXFEED; ++i)
	{
		HBRUSH myBrush = (HBRUSH)CreateSolidBrush(Brushes[i]);
		HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, myBrush);
		if (Data[i].Radius > 0)
			Ellipse(hdc, Data[i].Center.x - Data[i].Radius,
				Data[i].Center.y - Data[i].Radius,
				Data[i].Center.x + Data[i].Radius,
				Data[i].Center.y + Data[i].Radius);

		SelectObject(hdc, oldBrush);
		DeleteObject(myBrush);
	}
}