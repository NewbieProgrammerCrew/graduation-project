#pragma once
class Jelly
{
	bool		_in_use;

	int			_type;

	float		_pos_x;
	float		_pos_y;
	float		_pos_z;

	float		_extent_x;
	float		_extent_y;
	float		_extent_z;

	float		_yaw;
	float		_roll;
	float		_pitch;

	int			_index;

public:
	Jelly() {};
	Jelly(int type,
		float posX, float posY, float posZ,
		float extentX, float extentY, float extentZ,
		float yaw, float roll, float pitch,
		int index);
	bool InUse();
	void ChangeInUseState(bool state);
};

