#include "pch.h"
#include "Map.h"

Map::Map()
{
}

void Map::Draw(HDC hdc)
{
	int Grid_x = MAP_WIDTH / 100;
	int Grid_y = MAP_HEIGHT / 100;

	for (int i = 0; i < 100; ++i)
	{
		MoveToEx(hdc, Grid_x * i, 0, NULL);
		LineTo(hdc, Grid_x * i, MAP_HEIGHT);
		
		MoveToEx(hdc, 0, Grid_y * i, NULL);
		LineTo(hdc, MAP_WIDTH, Grid_y * i);
	}
}