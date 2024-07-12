#include "lSession.h"

void lSession::SendConnectedPacket()
{
	LOBBY_SERVER_CONNECTED_PACKET p;
	p.size = sizeof(LOBBY_SERVER_CONNECTED_PACKET);
	p.type = LOBBY_SERVER_CONNECTED;
	do_send(&p);
}

void lSession::do_recv()
{
	DWORD recv_flag = 0;
	memset(&recv_over.over, 0, sizeof(recv_over.over));
	recv_over.wsabuf.len = BUF_SIZE - prev_remain_;
	recv_over.wsabuf.buf = recv_over.send_buf + prev_remain_;
	WSARecv(socket_, &recv_over.wsabuf, 1, 0, &recv_flag,
		&recv_over.over, 0);
}
void lSession::do_send(void* packet)
{
	OVER_EXP* sdata = new OVER_EXP{ reinterpret_cast<char*>(packet) };
	WSASend(socket_, &sdata->wsabuf, 1, 0, 0, &sdata->over, 0);
}

void lSession::SendCreateRoomPacket(int chaser, int runners[MAX_RUNNER_NUM])
{
	CREATE_ROOM_PACKET p;
	p.size = sizeof(p);
	p.type = CREATE_ROOM;
	p.chaser = chaser;
	for (int i = 0; i < MAX_RUNNER_NUM; ++i) {
		p.runners[i] = runners[i];
	}
	do_send(&p);
}