#pragma once
#include "Global.h"
class FuseBox
{
public:
	int			map_num_;
	string		obj_name_;
	int			index_;
	int			color_;
	bool		active_;
	int			match_index_;
	int			type_;
	double		pos_x_;
	double		pos_y_;
	double		pos_z_;
	double		extent_x_;
	double		extent_y_;
	double		extent_z_;
	double		yaw_;
	double		roll_;
	double		pitch_;
	double		progress_;
	int			interaction_id_;

	FuseBox() {
		index_ = -1;
		color_ = -1;
		active_ = false;
		match_index_ = -1;
		progress_ = 0;
		interaction_id_ = -1;
	}
	~FuseBox() {}
};

