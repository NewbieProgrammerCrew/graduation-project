#pragma once

#include "Global.h"
#include "Gun.h"

class cIngameData {
public:
	int					room_num_;
	float				x_, y_, z_, r_;			// ĳ������ ��ǥ
	float				rx_, ry_, rz_;			// ĳ������ �ݰ�
	int					role_;					// 1~5 ������, 6~7 ���θ�
	int					hp_;					// ü��
	int					before_hp_;				// ���� ü�� ( ������ �˻�� )
	std::string			user_name_;				// ���� �̸�
	bool				die_;					// ��� ����
	float				speed_;					// �̵� �ӵ�
	bool				jump_;					// ���� ����
	bool				interaction_;			// ��ȣ�ۿ� ������ Ȯ��
	int					fuse_;					// ���� �������ִ� ǻ���� �ε���, ������ -1
	Gun					gun_;					// ���� ������� �� ����
	int					pre_gun_type_;			// ������ ����ߴ� ���� Ÿ��
	int					my_client_num_;			// �� Ŭ���̾�Ʈ ��ȣ
	int					my_ingame_num_;			// �� �ΰ��� �� ��ȣ
	int					damage_inflicted_on_enemy_;	// ������ �� �� ���ط�
	bool				in_use_;
	std::vector<int>	col_area_;				// ĳ������ �浹ó�� ��� ����
	
	cIngameData() {
		in_use_ = true;
		die_ = false;
		interaction_ = false;
		pre_gun_type_ = -1;
		fuse_ = -1;
	};
	~cIngameData() {};
};