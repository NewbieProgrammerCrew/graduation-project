#pragma once
#include "OVER_EXP.h"
#include "protocol.h"
#include "stdafx.h"
#include "types.h"
#include "Gun.h"

class SESSION {
	OVER_EXP _recv_over;

public:
	bool				in_use;
	int					_id;
	SOCKET				_socket;
	float				x, y, z, r;
	float				rx, ry, rz;
	float				speed;
	bool				jump;
	char				_role[PROTOCOL_NAME_SIZE];
	int					charactorNum;
	int					map_id;
	int					_hp;
	int					_money;
	std::string			_userName;
	bool				_ready;
	bool				_die;
	int					_prev_remain;
	float				extentX, extentY, extentZ; 
	std::vector<int>	ColArea;
	int					fuse;
	Gun					gun;
	bool				interaction;
	int					preGunType;
	int					chaserID;
	float				resurrectionCooldown;
	bool				chaserDie;


public:
	SESSION();
	~SESSION();
	void do_recv();
	void do_send(void* packet);
	void send_login_info_packet();
	void send_login_fail_packet();
	void send_move_packet(int c_id);
	void send_attack_packet(int c_id);
	void send_dead_packet(int c_id);
	void send_hitted_packet(int c_id);
	void send_pickup_fuse_packet(int c_id, int index);
	void send_pickup_gun_packet(int c_id, int _gun_type, int _item_box_index, int _left_gun_type);
	void send_use_gun_packet(int c_id);
	void send_fuse_box_active_packet(int index);
	void send_half_portal_gauge_packet();
	void send_max_portal_gauge_packet();
	void send_remove_jelly_packet(int jellyIndex);
	void send_idle_state_packet(int c_id);
	void send_aim_state_packet(int c_id);
	void send_opening_item_box_packet(int c_id, int index, float progress);
	void send_opening_fuse_box_packet(int c_id, int index, float progress);
	void send_item_box_opened_packet(int index, int _gun_type);
	void send_fuse_box_opened_packet(int index);
	void send_not_interactive_packet();
	void send_stop_open_packet(int c_id, int item, int index, float progress);
	void send_chaser_resurrection_packet(int c_id);
};

