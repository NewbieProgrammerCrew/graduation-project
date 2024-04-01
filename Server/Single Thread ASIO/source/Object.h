#pragma once

#include "Global.h"

class Object
{
public:
	bool			in_use_;
	int				map_num_;
	int				type_;
	std::string		obj_name_;
	double			pos_x_;
	double			pos_y_;
	double			pos_z_;
	double			extent_x_;
	double			extent_y_;
	double			extent_z_;
	double			yaw_;
	double			roll_;
	double			pitch_;


	Object() {
		in_use_ = false;
		type_ = NULL;
		obj_name_ = "";
	}
	~Object() {
		in_use_ = false;
	};
};