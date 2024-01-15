#pragma once

#include "Global.h"
#include "Over_Exp.h"

enum S_STATE { ST_FREE, ST_ALLOC, ST_INGAME };

class Session;
extern array<Session, MAX_USER> clients;


class Session {
	OVER_EXP _recv_over;

public:
	mutex _s_lock;
	S_STATE _state;
	int _id;
	SOCKET _socket;
	short	x, y;
	char	_name[NAME_SIZE];
	int		_prev_remain;
	int		_last_move_time;

public:
	Session()
	{
		_id = -1;
		_socket = 0;
		x = y = 0;
		_name[0] = 0;
		_state = ST_FREE;
		_prev_remain = 0;
	}

	~Session() {}

	void do_recv();
	void do_send(void* packet);
	void send_login_info_packet();
	void send_move_packet(int c_id);
	void send_add_player_packet(int c_id);
	void send_remove_player_packet(int c_id);
};