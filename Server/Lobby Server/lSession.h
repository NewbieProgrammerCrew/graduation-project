#pragma once
#include "Global.h"
#include "Over_Exp.h"
class lSession
{
	OVER_EXP	recv_over;

public:
	mutex		s_lock_;
	int			id_;
	SOCKET		socket_;
	int			prev_remain_;
	int			last_move_time_;
	std::string		user_name_;


public:
	lSession()
	{
		id_ = -1;
		socket_ = 0;
		prev_remain_ = 0;
	}

	~lSession() {}

	void SendConnectedPacket();
	void SendCreateRoomPacket(int chaser, int runners[MAX_RUNNER_NUM]);

	void do_recv();
	void do_send(void* packet);
};

