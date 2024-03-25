#pragma once
class Bomb
{
public:
	int		bomb_type_;		// Type of bomb -> -1 : default, 0 : stun, 1 : explosion, 
	
public:
	Bomb() {
		bomb_type_ = -1;
	}

	Bomb(int type) :bomb_type_(type) {}
};