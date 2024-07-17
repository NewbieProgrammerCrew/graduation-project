#pragma once

#include "Global.h"

class Object
{
public:
	bool			in_use_;
	int				map_num_;
	int				type_;
	std::string		obj_name_;
	float			pos_x_;
	float			pos_y_;
	float			pos_z_;
	float			extent_x_;
	float			extent_y_;
	float			extent_z_;
	float			yaw_;
	float			roll_;
	float			pitch_;
	int				index_;


	Object() {
		in_use_ = false;
		type_ = NULL;
		obj_name_ = "";
	}
	~Object() {
		in_use_ = false;
	};
};