#include "Session.h"

array<Session, MAX_USER> clients;


void Session::do_recv()
{
	DWORD recv_flag = 0;
	memset(&recv_over.over, 0, sizeof(recv_over.over));
	recv_over.wsabuf.len = BUF_SIZE - prev_remain;
	recv_over.wsabuf.buf = recv_over.send_buf + prev_remain;
	WSARecv(socket, &recv_over.wsabuf, 1, 0, &recv_flag,
		&recv_over.over, 0);
}
void Session::do_send(void* packet)
{
	OVER_EXP* sdata = new OVER_EXP{ reinterpret_cast<char*>(packet) };
	WSASend(socket, &sdata->wsabuf, 1, 0, 0, &sdata-> over, 0);
}

//void Session::send_login_info_packet()
//{
//	SC_LOGIN_INFO_PACKET p;
//	p.id = id;
//	p.size = sizeof(SC_LOGIN_INFO_PACKET);
//	p.type = SC_LOGIN_INFO;
//	do_send(&p);
//}

