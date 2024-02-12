#pragma once

#include "Global.h"

class Object
{
public:
	bool	in_use;
	int     map_num;
	int		type;
	string	obj_name;
	float	pos_x;
	float	pos_y;
	float	pos_z;
	float	extent_x;
	float	extent_y;
	float	extent_z;
	float	yaw;
	float	roll;
	float	pitch;


	Object() {
		in_use = false;
		type = NULL;
		obj_name = "";
	}
	~Object() {
		in_use = false;
	};
};