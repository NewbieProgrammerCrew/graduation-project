#pragma once

#include "Global.h"
#include "Over_Exp.h"

enum S_STATE { ST_FREE, ST_ALLOC, ST_INGAME };

class Session;
extern array<Session, MAX_USER> clients;


class Session {
	OVER_EXP	recv_over;

public:
	mutex		s_lock;
	S_STATE		state;
	int			id;
	SOCKET		socket;
	int			prev_remain;
	int			last_move_time;

public:
	Session()
	{
		id = -1;
		socket = 0;
		state = ST_FREE;
		prev_remain = 0;
	}

	~Session() {}

	void do_recv();
	void do_send(void* packet);
};