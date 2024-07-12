#define _CRT_SECURE_NO_WARNINGS
#include "Session.h"



void Session::SendLoginFailPacket()
{
	SC_LOGIN_FAIL_PACKET p;
	p.id = id_;
	p.size = sizeof(SC_LOGIN_FAIL_PACKET);
	p.type = SC_LOGIN_FAIL;
	p.errorCode = 102;
	do_send(&p);
}

void Session::SendLoginInfoPacket()
{
	SC_LOGIN_INFO_PACKET p;
	p.id = id_;
	p.size = sizeof(SC_LOGIN_INFO_PACKET);
	p.type = SC_LOGIN_INFO;
	std::string user_name = user_name_;
	strcpy(p.userName, user_name.c_str());

	do_send(&p);
}

void Session::SendGameStartPacket(int port_num)
{
	cout << "sendGameStart To Client " << id_ << "Port << " << port_num << "\n";
	SC_GAME_START_PACKET p;
	p.size = sizeof(SC_GAME_START_PACKET);
	p.type = SC_GAME_START;
	strcpy_s(p.address, "127.0.0.1");
	p.portNum = port_num;
	do_send(&p);
}

void Session::do_recv()
{
	DWORD recv_flag = 0;
	memset(&recv_over.over, 0, sizeof(recv_over.over));
	recv_over.wsabuf.len = BUF_SIZE - prev_remain_;
	recv_over.wsabuf.buf = recv_over.send_buf + prev_remain_;
	WSARecv(socket_, &recv_over.wsabuf, 1, 0, &recv_flag,
		&recv_over.over, 0);
}
void Session::do_send(void* packet)
{
	OVER_EXP* sdata = new OVER_EXP{ reinterpret_cast<char*>(packet) };
	WSASend(socket_, &sdata->wsabuf, 1, 0, 0, &sdata->over, 0);
}


