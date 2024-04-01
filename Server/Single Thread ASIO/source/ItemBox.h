#pragma once
#include "Bomb.h"

class ItemBox
{
public:
	Bomb	bomb_;
	double	progress_;
	int		interaction_id_;
public:
	ItemBox() {
		progress_ = 0;
		interaction_id_ = -1;
		bomb_.bomb_type_ = 1;
	};
};
