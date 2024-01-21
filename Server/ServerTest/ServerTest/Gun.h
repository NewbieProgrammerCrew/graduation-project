#pragma once
class Gun
{
	int		gunType;		// 어떤 총인지 -> 0 : 기절, 1 : 폭발, 2 : 먹물, -1  총이 없는상태;
public:
	Gun() {};
    Gun(int type);
	void ChangeGunType(int type);
	int GetGunType();
};

