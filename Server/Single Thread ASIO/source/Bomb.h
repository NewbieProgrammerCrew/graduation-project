#pragma once
class Bomb
{
public:
	bool	in_use_;
	double	x_, y_, z_;
	double	speed_;
	double	yaw_, pitch_;
	int		bomb_type_;		// Type of bomb -> -1 : default, 0 : stun, 1 : explosion
	
public:
	Bomb() {
		in_use_ = true;
		bomb_type_ = -1;
		x_ = y_ = z_ = 0;
		speed_ = 1600;
	}

	Bomb(int type) :bomb_type_(type) {
		in_use_ = true;
		x_ = y_ = z_ = 0;
		speed_ = 1600;
	}
};