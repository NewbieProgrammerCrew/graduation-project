#pragma once

#include "Global.h"
#include "Bomb.h"

class cIngameData {
public:
	int					map_num_;
	int					room_num_;
	float				x_, y_, z_, r_;			// 캐릭터의 좌표
	float				rx_, ry_, rz_;			// 캐릭터의 반경
	float				extent_z_;
	float				pitch_;
	int					role_;					// 1~5 생존자, 6~7 살인마
	int					hp_;					// 체력
	int					before_hp_;				// 이전 체력 ( 데미지 검사용 )
	std::string			user_name_;				// 유저 이름
	bool				die_;					// 사망 여부
	float				speed_;					// 이동 속도
	bool				jump_;					// 점프 여부
	bool				interaction_;			// 상호작용 중인지 확인
	int					fuse_;					// 지금 가지고있는 퓨즈의 인덱스, 없으면 -1
	BombType			bomb_type_;					// 지금 사용중인 폭탄 정보
	int					bomb_index_;
	int					pre_bomb_type_;			// 이전에 사용했던 총의 타입
	int					my_client_num_;			// 내 클라이언트 번호
	int					my_ingame_num_;			// 내 인게임 내 번호
	int					damage_inflicted_on_enemy_;	// 적에게 준 총 피해량
	bool				in_use_;
	std::vector<int>	col_area_;				// 캐릭터의 충돌처리 계산 범위
	std::chrono::high_resolution_clock::time_point		last_skill_time;
	int					skill_cool_down_;
	float				resurrectionCooldown_;
	int					resurrectionCount;
	int					score_;
	bool				Invincible = false;
	bool				damage_up_ = false;

	
	cIngameData() {
		in_use_ = true;
		die_ = false;
		interaction_ = false;
		pre_bomb_type_ = -1;
		bomb_type_ = NoBomb;
		fuse_ = -1;
		bomb_index_ = -1;
		last_skill_time = std::chrono::high_resolution_clock::now();
		skill_cool_down_ = 3;
		resurrectionCooldown_ = 10;
		resurrectionCount = 0;
		before_hp_ = 600;
		score_ = 0;
		Invincible = false;
		x_ = y_ = z_ = 0.0f;
	};
	~cIngameData() {};
};