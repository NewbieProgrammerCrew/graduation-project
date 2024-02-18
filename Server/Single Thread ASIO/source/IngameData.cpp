#include "IngameData.h"

int cIngameData::GetRoomNumber()
{
	return _room_num;
}

float cIngameData::GetPositionX()
{
	return _x;
}

float cIngameData::GetPositionY()
{
	return _y;
}

float cIngameData::GetPositionZ()
{
	return _z;
}

int cIngameData::GetHp()
{
	return _hp;
}

int cIngameData::GetRole()
{
	return _role;
}

std::string cIngameData::GetMyName()
{
	return _user_name;
}

bool cIngameData::GetDieState()
{
	return _die;
}

bool cIngameData::GetInteractionState()
{
	return _interaction;
}

int cIngameData::GetMyClientNumber()
{
	return _my_client_num;
}

int cIngameData::GetMyIngameNumber()
{
	return _my_ingame_num;
}

void cIngameData::SetRoomNumber(int roomNum)
{
	_room_num = roomNum;
}

void cIngameData::SetPosition(float x, float y, float z)
{
	_x = x;
	_y = y;
	_z = z;
}

void cIngameData::SetHp(int hp)
{
	_hp = hp;
}

void cIngameData::SetRole(int role)
{
	_role = role;
}

void cIngameData::SetUserName(string userName)
{
	_user_name = userName;
}

void cIngameData::SetDieState(bool die)
{
	_die = die;
}

void cIngameData::SetInteractionState(bool interaction)
{
	_interaction = interaction;
}

void cIngameData::SetMyClientNumber(int num)
{
	_my_client_num = num;
}

void cIngameData::SetMyIngameNum(int num)
{
	_my_ingame_num = num;
}
