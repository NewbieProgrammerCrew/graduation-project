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
	void send_pickup_gun_packet(int c_id, int _gun_type);
	void send_use_gun_packet(int c_id);
	void send_fuse_box_active_packet(int index);
	void send_half_portal_gauge_packet();
	void send_max_portal_gauge_packet();
};

