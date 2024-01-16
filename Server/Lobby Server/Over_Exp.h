#pragma once

#include "Global.h"

enum COMP_TYPE { OP_ACCEPT, OP_RECV, OP_SEND };

class OVER_EXP {
public:
	WSAOVERLAPPED	over;
	WSABUF			wsabuf;
	char			send_buf[BUF_SIZE];
	COMP_TYPE		comp_type;
	OVER_EXP()
	{
		wsabuf.len = BUF_SIZE;
		wsabuf.buf = send_buf;
		comp_type = OP_RECV;
		ZeroMemory(&over, sizeof(over));
	}
	OVER_EXP(char* packet)
	{
		wsabuf.len = packet[0];
		wsabuf.buf = send_buf;
		ZeroMemory(&over, sizeof(over));
		comp_type = OP_SEND;
		memcpy(send_buf, packet, packet[0]);
	}
};