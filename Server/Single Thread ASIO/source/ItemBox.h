#pragma once
#include "Gun.h"

class ItemBox
{
public:
	Gun		gun_;
	float	progress_;
	int		interaction_id_;
public:
	ItemBox() {
		progress_ = 0;
		interaction_id_ = -1;
		gun_.gun_type_ = 1;
	};
	ItemBox(Gun gun);
};
