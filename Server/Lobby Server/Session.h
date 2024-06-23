#pragma once

#include "Global.h"
#include "Over_Exp.h"

enum S_STATE { ST_FREE, ST_ALLOC, ST_INGAME };

class Session {
	OVER_EXP	recv_over;

public:
	mutex		s_lock_;
	S_STATE		state_;
	int			id_;
	SOCKET		socket_;
	int			prev_remain_;
	int			last_move_time_;
	std::string		user_name_;
	char			role_[20];
	int				charactor_num_;
	bool			ready_;

public:
	Session()
	{
		id_ = -1;
		socket_ = 0;
		state_ = ST_FREE;
		prev_remain_ = 0;
	}

	~Session() {}

	void SendLoginFailPacket();
	void SendLoginInfoPacket();
	void SendGameStartPacket(int port_num);

	void do_recv();
	void do_send(void* packet);
};