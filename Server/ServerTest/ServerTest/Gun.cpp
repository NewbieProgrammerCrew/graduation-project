#include "Gun.h"

Gun::Gun(int type)
{
	gunType = type;
}

void Gun::ChangeGunType(int type)
{
	gunType = type;
}

int Gun::GetGunType()
{
	return gunType;
}
