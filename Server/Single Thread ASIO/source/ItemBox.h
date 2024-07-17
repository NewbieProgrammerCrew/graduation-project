#pragma once
#include "Bomb.h"

class ItemBox
{
public:
	Bomb	bomb_;
	int		index_;
	float	progress_;
	int		interaction_id_;
public:
	ItemBox() {
		bomb_.index_ = index_;
		progress_ = 0;
		interaction_id_ = -1;
		bomb_.bomb_type_ = NoBomb;
	};
};
