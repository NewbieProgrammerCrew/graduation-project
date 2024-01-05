#include "Jelly.h"

Jelly::Jelly(int _type, float _posX, float _posY, float _posZ, float _extentX, float _extentY, float _extentZ, float _yaw, float _roll, float _pitch, int _index)
{
	inUse = true;

	type = _type;
	posX = _posX;
	posY = _posY;
	posZ = _posZ;

	extentX = _extentX;
	extentY = _extentY;
	extentZ = _extentZ;

	yaw = _yaw;
	roll = _roll;
	pitch = _pitch;

	index = _index;
}

bool Jelly::InUse()
{
	return inUse;
}

void Jelly::ChangeInUseState(bool state)
{
	inUse = state;
}
