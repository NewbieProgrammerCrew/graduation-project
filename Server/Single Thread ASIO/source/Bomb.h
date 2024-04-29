#pragma once

#include "Global.h"

class Bomb
{
public:
	bool		in_use_;
	Vector3D	pos_;
	Vector3D	initialVelocity_;
	int			index_;
	double		rx_, ry_, rz_;
	double		r_;
	double		speed_;
	double		yaw_, pitch_;
	BombType	bomb_type_;		// Type of bomb -> -1 : default, 0 : stun, 1 : explosion
	
public:
	Bomb() {
		in_use_ = true;
		bomb_type_ = NoBomb;
		pos_.x = pos_.y=pos_.z = 0;
		speed_ = 1600;
		r_ = 0.5358935;
	}

	Bomb(BombType type) :bomb_type_(type) {
		in_use_ = true;
		pos_.x = pos_.y = pos_.z = 0;
		speed_ = 1600;
		r_ = 0.5358935;
	}

	Vector3D calculateInitialVelocity() {
		double vx = speed_ * rx_;
		double vy = speed_ * ry_;
		double vz = speed_ * rz_;

		return { vx, vy, vz };
	}
};

