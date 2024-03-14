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

float cIngameData::GetRadian()
{
	return _r;
}

float cIngameData::GetRotationValueX()
{
	return _rx;
}

float cIngameData::GetRotationValueY()
{
	return _ry;
}

float cIngameData::GetRotationValueZ()
{
	return _rz;
}

float cIngameData::GetSpeed()
{
	return _speed;
}

bool cIngameData::GetJump()
{
	return _jump;
}

int cIngameData::GetFuseIndex()
{
	return _fuse;
}

int cIngameData::GetGunType()
{
	return _gun.GetGunType();
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

void cIngameData::SetRotationValue(float rx, float ry, float rz)
{
	_rx = rx;
	_ry = ry;
	_rz = rz;
}

void cIngameData::SetSeppd(float speed)
{
	_speed = speed;
}

void cIngameData::SetJump(bool jump)
{
	_jump = jump;
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

void cIngameData::SetRadian(float r)
{
	_r = r;
}

void cIngameData::SetFuseIndex(int index)
{
	_fuse = index;
}

void cIngameData::SetGunType(int type)
{
	_gun.ChangeGunType(type);
}

void cIngameData::ChangeDamagenIflictedOnEnemy(int damage)
{
	_damage_inflicted_on_enemy += damage;
}

void cIngameData::ChangeHp(int hp)
{
	_hp += hp;
}
