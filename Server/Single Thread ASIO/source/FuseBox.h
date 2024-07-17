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
	float		pos_x_;
	float		pos_y_;
	float		pos_z_;
	float		extent_x_;
	float		extent_y_;
	float		extent_z_;
	float		yaw_;
	float		roll_;
	float		pitch_;
	float		progress_;
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

