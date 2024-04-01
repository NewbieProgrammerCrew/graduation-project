#pragma once
class Jelly
{
public:
	bool		in_use_;

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

	int			index_;

public:
	Jelly() {};
	Jelly(int type,
		double posX, double posY, double posZ,
		double extentX, double extentY, double extentZ,
		double yaw, double roll, double pitch,
		int index);
	bool InUse();
};

