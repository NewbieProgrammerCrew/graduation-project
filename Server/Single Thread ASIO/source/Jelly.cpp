#include "Jelly.h"

Jelly::Jelly(int type, float posX, float posY, float posZ, float extentX, float extentY, float extentZ, float yaw, float roll, float pitch, int index)
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