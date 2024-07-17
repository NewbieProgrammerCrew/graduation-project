#pragma once
class Jelly
{
public:
	bool		in_use_;

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

	int			index_;

public:
	Jelly() {
		in_use_ = true;
	};
	Jelly(int type,
		float posX, float posY, float posZ,
		float extentX, float extentY, float extentZ,
		float yaw, float roll, float pitch,
		int index);
};

