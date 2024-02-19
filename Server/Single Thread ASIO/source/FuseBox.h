#pragma once
#include "Global.h"
class FuseBox
{
public:
	int			_map_num;
	string		_obj_name;
	int			_index;
	int			_color;
	bool		_active;
	int			_match_index;
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
	float		_progress;
	int			_interaction_id;

	FuseBox() {
		_index = -1;
		_color = -1;
		_active = false;
		_match_index = -1;
		_progress = 0;
		_interaction_id = -1;
	}
	~FuseBox() {}
};

