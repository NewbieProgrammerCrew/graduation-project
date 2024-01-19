#pragma once
class Gun
{
	int		gunType;		// ¾î¶² ÃÑÀÎÁö -> 0 : ±âÀý, 1 : Æø¹ß, 2 : ¸Ô¹°
public:
	Gun() {};
    Gun(int type);
	void ChangeGunType(int type);
	int GetGunType();
};

