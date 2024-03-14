#pragma once
#include "Gun.h"

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
		gun.ChangeGunType(-1);
	};
	ItemBox(Gun _gun);
	
	int GetGunType() { return gun.GetGunType(); }

	void SetGunType(int type) { gun.ChangeGunType(type); }
};
