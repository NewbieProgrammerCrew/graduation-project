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
	int			bomb_type_;		// Type of bomb -> -1 : default, 0 : stun, 1 : explosion
	
public:
	Bomb() {
		in_use_ = true;
		bomb_type_ = -1;
		pos_.x = pos_.y=pos_.z = 0;
		speed_ = 1600;
		r_ = 0.5358935;
	}

	Bomb(int type) :bomb_type_(type) {
		in_use_ = true;
		pos_.x = pos_.y = pos_.z = 0;
		speed_ = 1600;
		r_ = 0.5358935;
	}
	Bomb operator=(Bomb other) {
		in_use_ = other.in_use_;
		pos_ = other.pos_;
		initialVelocity_ = other.initialVelocity_;
		index_ = other.index_;
		rx_ = other.rx_;
		ry_ = other.ry_;
		rz_ = other.rz_;
		r_ = other.r_;
		speed_ = other.speed_;
		yaw_ = other.yaw_;
		pitch_ = other.pitch_;
		bomb_type_ = other.bomb_type_;
		return *this;
	}
	Vector3D calculateInitialVelocity() {
		double vx = speed_ * rx_;
		double vy = speed_ * ry_;
		double vz = speed_ * rz_;

		return { vx, vy, vz };
	}
};

