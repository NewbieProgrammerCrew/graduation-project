#pragma once

#include "Global.h"
#include "Gun.h"

class cIngameData {
private:
	int					_room_num;
	float				_x, _y, _z, _r;			// ĳ������ ��ǥ
	float				_rx, _ry, _rz;			// ĳ������ �ݰ�
	int					_role;					// 0 : ����, 1 : ������
	int					_hp;					// ü��
	int					_before_hp;				// ���� ü�� ( ������ �˻�� )
	std::string			_user_name;				// ���� �̸�
	bool				_die;					// ��� ����
	float				_speed;					// �̵� �ӵ�
	bool				_jump;					// ���� ����
	bool				_interaction;			// ��ȣ�ۿ� ������ Ȯ��
	int					_fuse;					// ���� �������ִ� ǻ���� �ε���, ������ -1
	Gun					_gun;					// ���� ������� �� ����
	int					_pre_gun_type;			// ������ ����ߴ� ���� Ÿ��
	int					_my_client_num;			// �� Ŭ���̾�Ʈ ��ȣ
	int					_my_ingame_num;			// �� �ΰ��� �� ��ȣ





public:
	bool _in_use;
	std::vector<int>	_col_area;				// ĳ������ �浹ó�� ��� ����
	
	cIngameData() {
		_in_use = true;
		_die = false;
		_interaction = false;
		_pre_gun_type = -1;
	};
	~cIngameData() {};

	// Getter
	int GetRoomNumber();
	float GetPositionX();
	float GetPositionY();
	float GetPositionZ();
	int GetHp();
	int GetRole();
	std::string GetMyName();
	bool GetDieState();
	bool GetInteractionState();
	int GetMyClientNumber();
	int GetMyIngameNumber();
	int GetRadian();
	float GetRotationValueX();
	float GetRotationValueY();
	float GetRotationValueZ();
	float GetSpeed();
	bool GetJump();



	// Setter
	void SetRoomNumber(int roomNum);
	void SetPosition(float x, float y, float z);
	void SetRotationValue(float rx, float ry, float rz);
	void SetSeppd(float speed);
	void SetJump(bool jump);
	void SetHp(int hp);
	void SetRole(int role);
	void SetUserName(string userName);
	void SetDieState(bool die);
	void SetInteractionState(bool interaction);
	void SetMyClientNumber(int num);
	void SetMyIngameNum(int num);
	void SetRadian(int r);

};