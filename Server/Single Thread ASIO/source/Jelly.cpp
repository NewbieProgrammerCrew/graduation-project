#include "Jelly.h"

Jelly::Jelly(int type, float posX, float posY, float posZ, float extentX, float extentY, float extentZ, float yaw, float roll, float pitch, int index)
{
	_in_use = true;

	_type = type;
	_pos_x = posX;
	_pos_y = posY;
	_pos_z = posZ;

	_extent_x = extentX;
	_extent_y = extentY;
	_extent_z = extentZ;

	_yaw = yaw;
	_roll = roll;
	_pitch = pitch;

	_index = index;
}

bool Jelly::InUse()
{
	return _in_use;
}

void Jelly::ChangeInUseState(bool state)
{
	_in_use = state;
}
