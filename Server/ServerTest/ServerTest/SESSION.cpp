#include "SESSION.h"
#include "Global.h"

array<SESSION, MAX_USER> clients;
array<SESSION, MAX_ITEM> items;

SESSION::SESSION() : _socket(0), in_use(false)
{
	_id = -1;
	x = y = z = 0;
	rx = ry = rz = 0;
	_role[0] = 0;
	_prev_remain = 0;
	_ready = false;
	_die = false;
	extentX = 12.7857;
	extentY = 16.6400;
	extentZ = 45.1027;
	fuse = -1;
	gun = NULL;
	interaction = false;
	charactorNum = -1;
}

SESSION::~SESSION() {}

void SESSION::do_recv()
{
	DWORD recv_flag = 0;
	memset(&_recv_over._over, 0, sizeof(_recv_over._over));
	_recv_over._wsabuf.len = BUF_SIZE - _prev_remain;
	_recv_over._wsabuf.buf = _recv_over._send_buf + _prev_remain;
	WSARecv(_socket, &_recv_over._wsabuf, 1, 0, &recv_flag,
		&_recv_over._over, 0);
}

void SESSION::do_send(void* packet)
{
	OVER_EXP* sdata = new OVER_EXP{ reinterpret_cast<char*>(packet) };
	WSASend(_socket, &sdata->_wsabuf, 1, 0, 0, &sdata->_over, 0);
}

void SESSION::send_login_info_packet()
{
	SC_LOGIN_INFO_PACKET p;
	p.id = _id;
	p.size = sizeof(SC_LOGIN_INFO_PACKET);
	p.type = SC_LOGIN_INFO;
	strcpy(p.userName, _userName.c_str());
	p.money = _money;

	do_send(&p);
}

void SESSION::send_login_fail_packet()
{
	SC_LOGIN_FAIL_PACKET p;
	p.id = _id;
	p.size = sizeof(SC_LOGIN_FAIL_PACKET);
	p.type = SC_LOGIN_FAIL;
	p.errorCode = 102;
	do_send(&p);
}

void SESSION::send_move_packet(int c_id)
{
	SC_MOVE_PLAYER_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_MOVE_PLAYER_PACKET);
	p.type = SC_MOVE_PLAYER;
	p.x = clients[c_id].x;
	p.y = clients[c_id].y;
	p.z = clients[c_id].z;

	p.rx = clients[c_id].rx;
	p.ry = clients[c_id].ry;
	p.rz = clients[c_id].rz;
	p.speed = clients[c_id].speed;
	p.jump = clients[c_id].jump;
	
	do_send(&p);
}

void SESSION::send_attack_packet(int c_id)
{
	SC_ATTACK_PLAYER_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_ATTACK_PLAYER_PACKET);
	p.type = SC_ATTACK_PLAYER;


	p.x = clients[c_id].x;
	p.y = clients[c_id].y;
	p.z = clients[c_id].z;
	p.ry = clients[c_id].ry;

	do_send(&p);
}

void SESSION::send_dead_packet(int c_id)
{
	SC_DEAD_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_DEAD_PACKET);
	p.type = SC_DEAD;
	p._hp = 0;

	do_send(&p);

}

void SESSION::send_hitted_packet(int c_id)
{
	SC_HITTED_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_HITTED_PACKET);
	p.type = SC_HITTED;
	p._hp = clients[c_id]._hp;

	do_send(&p);

}
void SESSION::send_pickup_fuse_packet(int c_id, int index)
{
	SC_PICKUP_FUSE_PACKET p;
	p.size = sizeof(SC_PICKUP_FUSE_PACKET);
	p.type = SC_PICKUP_FUSE;
	p.index = index;
	p.id = c_id;
	do_send(&p);
}

void SESSION::send_pickup_gun_packet(int c_id, int _gun_type, int _item_box_index, int _left_gun_type)
{
	SC_PICKUP_GUN_PACKET p;
	p.size = sizeof(SC_PICKUP_GUN_PACKET);
	p.type = SC_PICKUP_GUN;
	p.id = c_id;
	p.gun_type = _gun_type;
	p.itemBoxIndex = _item_box_index;
	p.leftGunType = _left_gun_type;
	do_send(&p);
}

void SESSION::send_use_gun_packet(int c_id)
{
	SC_USE_GUN_PACKET p;
	p.size = sizeof(SC_USE_GUN_PACKET);
	p.type = SC_USE_GUN;
	p.id = c_id;
	do_send(&p);
}

void SESSION::send_fuse_box_active_packet(int index)
{
	SC_FUSE_BOX_ACTIVE_PACKET p;
	p.size = sizeof(SC_FUSE_BOX_ACTIVE_PACKET);
	p.type = SC_FUSE_BOX_ACTIVE;
	p.fuseBoxIndex = index;
	do_send(&p);

}

void SESSION::send_half_portal_gauge_packet()
{
	SC_HALF_PORTAL_GAUGE_PACKET p;
	p.size = sizeof(SC_HALF_PORTAL_GAUGE_PACKET);
	p.type = SC_HALF_PORTAL_GAUGE;
	do_send(&p);

}

void SESSION::send_max_portal_gauge_packet()
{
	SC_MAX_PORTAL_GAUGE_PACKET p;
	p.size = sizeof(SC_MAX_PORTAL_GAUGE_PACKET);
	p.type = SC_MAX_PORTAL_GAUGE;
	do_send(&p);

}

void SESSION::send_remove_jelly_packet(int jellyIndex)
{
	SC_REMOVE_JELLY_PACKET p;
	p.size = sizeof(SC_REMOVE_JELLY_PACKET);
	p.type = SC_REMOVE_JELLY;
	p.jellyIndex = jellyIndex;
	do_send(&p);
}

void SESSION::send_idle_state_packet(int c_id)
{
	SC_IDLE_STATE_PACKET p;
	p.size = sizeof(SC_IDLE_STATE_PACKET);
	p.type = SC_IDLE_STATE;
	p.id = c_id;
	do_send(&p);
}

void SESSION::send_aim_state_packet(int c_id)
{
	SC_AIM_STATE_PACKET p;
	p.size = sizeof(SC_AIM_STATE_PACKET);
	p.type = SC_AIM_STATE;
	p.id = c_id;
	do_send(&p);
}

void SESSION::send_opening_item_box_packet(int c_id ,int index, float progress)
{
	SC_OPENING_ITEM_BOX_PACKET p;
	p.size = sizeof(SC_OPENING_ITEM_BOX_PACKET);
	p.type = SC_OPENING_ITEM_BOX;
	p.id = c_id;
	p.index = index;
	p.progress = progress;
	do_send(&p);
}

void SESSION::send_item_box_opened_packet(int index, int _gun_type)
{
	SC_ITEM_BOX_OPENED_PACKET p;
	p.size = sizeof(SC_ITEM_BOX_OPENED_PACKET);
	p.type = SC_ITEM_BOX_OPENED;
	p.index = index;
	p.gun_id = _gun_type;
	do_send(&p);
}

void SESSION::send_not_interactive_packet()
{
	SC_NOT_INTERACTIVE_PACKET p;
	p.size = sizeof(SC_NOT_INTERACTIVE_PACKET);
	p.type = SC_NOT_INTERACTIVE;
	do_send(&p);
}

