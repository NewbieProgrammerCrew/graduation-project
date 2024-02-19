#include "Gun.h"

void Gun::ChangeGunType(int type)
{
	_gun_type = type;
}

int Gun::GetGunType()
{
	return _gun_type;
}
