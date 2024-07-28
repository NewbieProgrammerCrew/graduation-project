#pragma once

#include "Global.h"
#include "Bomb.h"

class cIngameData {
public:
	int					map_num_;
	int					room_num_;
	float				x_, y_, z_, r_;			// ĳ������ ��ǥ
	float				rx_, ry_, rz_;			// ĳ������ �ݰ�
	float				extent_z_;
	float				pitch_;
	int					role_;					// 1~5 ������, 6~7 ���θ�
	int					hp_;					// ü��
	int					before_hp_;				// ���� ü�� ( ������ �˻�� )
	std::string			user_name_;				// ���� �̸�
	bool				die_;					// ��� ����
	float				speed_;					// �̵� �ӵ�
	bool				jump_;					// ���� ����
	bool				interaction_;			// ��ȣ�ۿ� ������ Ȯ��
	int					fuse_;					// ���� �������ִ� ǻ���� �ε���, ������ -1
	BombType			bomb_type_;					// ���� ������� ��ź ����
	int					bomb_index_;
	int					pre_bomb_type_;			// ������ ����ߴ� ���� Ÿ��
	int					my_client_num_;			// �� Ŭ���̾�Ʈ ��ȣ
	int					my_ingame_num_;			// �� �ΰ��� �� ��ȣ
	int					damage_inflicted_on_enemy_;	// ������ �� �� ���ط�
	bool				in_use_;
	std::vector<int>	col_area_;				// ĳ������ �浹ó�� ��� ����
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