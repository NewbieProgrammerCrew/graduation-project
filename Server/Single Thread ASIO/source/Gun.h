#pragma once
class Gun
{
public:
	int		gun_type_;		// � ������ -> 0 : ����, 1 : ����, 2 : �Թ�, -1  ���� ���»���;

public:
	Gun() {
		gun_type_ = -1;
	};
	Gun(int type) { gun_type_ = type; };
};

