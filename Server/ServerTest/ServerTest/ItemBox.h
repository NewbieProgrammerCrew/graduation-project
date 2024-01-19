#include "Gun.h"
#pragma once

class ItemBox
{
public:
	Gun		gun;
	float	progress;
public:
	ItemBox() {
		progress = 0;
	};
	ItemBox(Gun _gun);
};

