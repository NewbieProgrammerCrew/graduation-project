#pragma once
#include "types.h"
class FuseBox
{
public:
	int			map_num;
	string		obj_name;
	int			index;
	int			color;
	bool		active;
	int			matchIndex;
	int			type;
	float		pos_x;
	float		pos_y;
	float		pos_z;
	float		extent_x;
	float		extent_y;
	float		extent_z;
	float		yaw;
	float		roll;
	float		pitch;

	FuseBox() {
		index = -1;
		color = -1;
		active = false;
		matchIndex = -1;
	}
	~FuseBox() {}
};

