#pragma once
class Gun
{
public:
	int		_gun_type;		// � ������ -> 0 : ����, 1 : ����, 2 : �Թ�, -1  ���� ���»���;

public:
	Gun() {
		_gun_type = -1;
	};
	Gun(int type) { _gun_type = type; };
};

