#pragma once

#include "Global.h"
#include "Gun.h"

class cIngameData {
public:
	int					room_num_;
	float				x_, y_, z_, r_;			// 캐릭터의 좌표
	float				rx_, ry_, rz_;			// 캐릭터의 반경
	int					role_;					// 1~5 생존자, 6~7 살인마
	int					hp_;					// 체력
	int					before_hp_;				// 이전 체력 ( 데미지 검사용 )
	std::string			user_name_;				// 유저 이름
	bool				die_;					// 사망 여부
	float				speed_;					// 이동 속도
	bool				jump_;					// 점프 여부
	bool				interaction_;			// 상호작용 중인지 확인
	int					fuse_;					// 지금 가지고있는 퓨즈의 인덱스, 없으면 -1
	Gun					gun_;					// 지금 사용중인 총 정보
	int					pre_gun_type_;			// 이전에 사용했던 총의 타입
	int					my_client_num_;			// 내 클라이언트 번호
	int					my_ingame_num_;			// 내 인게임 내 번호
	int					damage_inflicted_on_enemy_;	// 적에게 준 총 피해량
	bool				in_use_;
	std::vector<int>	col_area_;				// 캐릭터의 충돌처리 계산 범위
	
	cIngameData() {
		in_use_ = true;
		die_ = false;
		interaction_ = false;
		pre_gun_type_ = -1;
		fuse_ = -1;
	};
	~cIngameData() {};
};