#pragma once
class Gun
{
	int		gunType;		// � ������ -> 0 : ����, 1 : ����, 2 : �Թ�
public:
	Gun() {};
    Gun(int type);
	void ChangeGunType(int type);
	int GetGunType();
};

