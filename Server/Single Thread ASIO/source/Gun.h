#pragma once
class Gun
{
public:
	int		gun_type_;		// 어떤 총인지 -> 0 : 기절, 1 : 폭발, 2 : 먹물, -1  총이 없는상태;

public:
	Gun() {
		gun_type_ = -1;
	};
	Gun(int type) { gun_type_ = type; };
};

