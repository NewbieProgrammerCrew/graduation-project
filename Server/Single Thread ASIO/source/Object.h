#pragma once

#include "Global.h"

class Object
{
public:
	bool	_in_use;
	int     _map_num;
	int		_type;
	string	_obj_name;
	float	_pos_x;
	float	_pos_y;
	float	_pos_z;
	float	_extent_x;
	float	_extent_y;
	float	_extent_z;
	float	_yaw;
	float	_roll;
	float	_pitch;


	Object() {
		_in_use = false;
		_type = NULL;
		_obj_name = "";
	}
	~Object() {
		_in_use = false;
	};
};