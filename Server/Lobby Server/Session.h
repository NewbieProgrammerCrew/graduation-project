#pragma once

#include "Global.h"
#include "Over_Exp.h"

enum S_STATE { ST_FREE, ST_ALLOC, ST_INGAME };

class Session;
extern array<Session, MAX_USER> clients;


class Session {
	OVER_EXP	recv_over;

public:
	mutex		s_lock_;
	S_STATE		state_;
	int			id_;
	SOCKET		socket_;
	int			prev_remain_;
	int			last_move_time_;

public:
	Session()
	{
		id_ = -1;
		socket_ = 0;
		state_ = ST_FREE;
		prev_remain_ = 0;
	}

	~Session() {}

	void do_recv();
	void do_send(void* packet);
};