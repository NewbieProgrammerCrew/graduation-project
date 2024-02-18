#pragma once

#include "Global.h"
#include "Gun.h"

class cIngameData {
private:
	int					_room_num;
	float				_x, _y, _z, _r;			// 캐릭터의 좌표
	float				_rx, _ry, _rz;			// 캐릭터의 반경
	int					_role;					// 0 : 술래, 1 : 도망자
	int					_hp;					// 체력
	int					_before_hp;				// 이전 체력 ( 데미지 검사용 )
	std::string			_user_name;				// 유저 이름
	bool				_die;					// 사망 여부
	float				_speed;					// 이동 속도
	bool				_jump;					// 점프 여부
	bool				_interaction;			// 상호작용 중인지 확인
	int					_fuse;					// 지금 가지고있는 퓨즈의 인덱스, 없으면 -1
	Gun					_gun;					// 지금 사용중인 총 정보
	int					_pre_gun_type;			// 이전에 사용했던 총의 타입
	int					_my_client_num;			// 내 클라이언트 번호
	int					_my_ingame_num;			// 내 인게임 내 번호





public:
	bool _in_use;
	std::vector<int>	_col_area;				// 캐릭터의 충돌처리 계산 범위
	
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