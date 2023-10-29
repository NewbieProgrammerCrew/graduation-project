#pragma once
#include "protocol.h"
#include "stdafx.h"

class OVER_EXP {
public:
	OVER_EXP();
	OVER_EXP(char* packet);
public:
	WSAOVERLAPPED _over;
	WSABUF _wsabuf;
	char _send_buf[BUF_SIZE];
	COMP_TYPE _comp_type;
};

