#pragma once
class Gun
{
	int		gunType;		// � ������ -> 0 : ����, 1 : ����, 2 : �Թ�, -1  ���� ���»���;
public:
	Gun() {};
    Gun(int type);
	void ChangeGunType(int type);
	int GetGunType();
};

