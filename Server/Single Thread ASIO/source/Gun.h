#pragma once
class Gun
{
	int		_gun_type;		// 어떤 총인지 -> 0 : 기절, 1 : 폭발, 2 : 먹물, -1  총이 없는상태;
public:
	Gun() {
		_gun_type = -1;
	};
	Gun(int type) {};
	void ChangeGunType(int type);
	int GetGunType();
};

