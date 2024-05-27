#include "Session.h"

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
	WSASend(socket_, &sdata->wsabuf, 1, 0, 0, &sdata-> over, 0);
}


