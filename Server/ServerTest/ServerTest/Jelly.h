#pragma once
class Jelly
{
	bool		inUse;

	int			type;

	float		posX;
	float		posY;
	float		posZ;

	float		extentX;
	float		extentY;
	float		extentZ;

	float		yaw;
	float		roll;
	float		pitch;

	int			index;

public:
	Jelly() {};
	Jelly(int _type,
		float _posX, float _posY, float _posZ,
		float _extentX, float _extentY, float _extentZ,
		float _yaw, float _roll, float _pitch,
		int _index);
	bool InUse();
	void ChangeInUseState(bool state);
};

