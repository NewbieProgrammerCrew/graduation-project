#include "Gun.h"
#pragma once

class ItemBox
{
public:
	Gun		gun;
	float	progress;
	int		interaction_id;
public:
	ItemBox() {
		progress = 0;
		interaction_id = -1;
	};
	ItemBox(Gun _gun);
};

