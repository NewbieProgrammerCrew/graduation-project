#include "Jelly.h"

Jelly::Jelly(int type, double posX, double posY, double posZ, double extentX, double extentY, double extentZ, double yaw, double roll, double pitch, int index)
{
	in_use_ = true;

	type_ = type;
	pos_x_ = posX;
	pos_y_ = posY;
	pos_z_ = posZ;

	extent_x_ = extentX;
	extent_y_ = extentY;
	extent_z_ = extentZ;

	yaw_ = yaw;
	roll_ = roll;
	pitch_ = pitch;

	index_ = index;
}

bool Jelly::InUse()
{
	return in_use_;
}