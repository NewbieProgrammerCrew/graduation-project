#define _USE_MATH_DEFINES
#include <iostream>
#include <array>
#include <random>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <math.h>
#include <map>
#include <set>
#include <cstdlib>
#include "protocol.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
using namespace std;
constexpr int MAX_USER = 10;

default_random_engine dre;
uniform_int_distribution<int> uid{1,3};

enum COMP_TYPE { OP_ACCEPT, OP_RECV, OP_SEND };
class OVER_EXP {
public:
	WSAOVERLAPPED _over;
	WSABUF _wsabuf;
	char _send_buf[BUF_SIZE];
	COMP_TYPE _comp_type;
	OVER_EXP()
	{
		_wsabuf.len = BUF_SIZE;
		_wsabuf.buf = _send_buf;
		_comp_type = OP_RECV;
		ZeroMemory(&_over, sizeof(_over));
	}
	OVER_EXP(char* packet)
	{
		_wsabuf.len = packet[0];
		_wsabuf.buf = _send_buf;
		ZeroMemory(&_over, sizeof(_over));
		_comp_type = OP_SEND;
		memcpy(_send_buf, packet, packet[0]);
	}
};

int		MAPID;
bool	CHANGEMAP = false;

class SESSION {
	OVER_EXP _recv_over;

public:
	bool			in_use;
	int				_id;
	int				_mapid;
	SOCKET			_socket;
	float			x, y, z;
	float			rx, ry, rz;
	float			speed;
	char			_role[PROTOCOL_NAME_SIZE];
	int				_hp;
	std::string		_userName;

	int				_prev_remain;
public:
	SESSION() : _socket(0), in_use(false)
	{
		_id = -1;
		x = y = z = 0;
		rx = ry = rz = 0;
		_role[0] = 0;
		_prev_remain = 0;
	}

	~SESSION() {}

	void do_recv()
	{
		DWORD recv_flag = 0;
		memset(&_recv_over._over, 0, sizeof(_recv_over._over));
		_recv_over._wsabuf.len = BUF_SIZE - _prev_remain;
		_recv_over._wsabuf.buf = _recv_over._send_buf + _prev_remain;
		WSARecv(_socket, &_recv_over._wsabuf, 1, 0, &recv_flag,
			&_recv_over._over, 0);
	}

	void do_send(void* packet)
	{
		OVER_EXP* sdata = new OVER_EXP{ reinterpret_cast<char*>(packet) };
		WSASend(_socket, &sdata->_wsabuf, 1, 0, 0, &sdata->_over, 0);
	}
	void send_login_info_packet()
	{
		SC_LOGIN_INFO_PACKET p;
		p.id = _id;
		p.size = sizeof(SC_LOGIN_INFO_PACKET);
		p.type = SC_LOGIN_INFO;
		p.userName = _userName;

		do_send(&p);
	}

	void send_login_fail_packet()
	{
		SC_LOGIN_FAIL_PACKET p;
		p.id = _id;
		p.size = sizeof(SC_LOGIN_FAIL_PACKET);
		p.type = SC_LOGIN_FAIL;
		p.errorCode = "아이디 또는 비밀번호를 확인해 주세요.";
		do_send(&p);
	}

	void send_move_packet(int c_id);
    void send_attack_packet(int c_id);
    void send_dead_packet(int c_id);
    void send_hitted_packet(int c_id);
};

array<SESSION, MAX_USER> clients;
map<std::string, array<std::string,2>> UserInfo;
map<std::string, std::string> UserInfo2;
set<std::string> UserName;

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

    //p.ry = clients[c_id].ry;

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

int get_new_client_id()
{
	for (int i = 0; i < MAX_USER; ++i)
		if (clients[i].in_use == false)
			return i;
	return -1;
}

void process_packet(int c_id, char* packet)
{
	switch (packet[1]) {
	case CS_LOGIN: {
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		if (UserInfo.find(p->id) == UserInfo.end()) {
			clients[c_id].send_login_fail_packet();
			
			break;
		}
		else if (UserInfo[p->id][0] != p->password) {
			clients[c_id].send_login_fail_packet();
			break;
		}
		clients[c_id]._userName = UserInfo[p->id][1];
		clients[c_id].send_login_info_packet();
		break;
	}
	case CS_SIGNUP: {
		CS_SIGNUP_PACKET* p = reinterpret_cast<CS_SIGNUP_PACKET*>(packet);

		SC_SIGNUP_PACKET* signupPacket;
		signupPacket->type = SC_SIGNUP;
		signupPacket->size = sizeof(SC_SIGNUP_PACKET);

		if (UserInfo.find(p->id) == UserInfo.end()) {	// 중복되는 아이디가 있는지 확인
		}
		else {

			signupPacket->success = false;
			signupPacket->errorCode = "이미 사용중인 아이디 입니다.";
			clients[c_id].do_send(&signupPacket);
			break;
		}

		if (UserName.contains(p->userName)) {	// 중복되는 닉네임이 있는지 확인.
			signupPacket->success = false;
			signupPacket->errorCode = "이미 사용중인 닉네임 입니다.";
			clients[c_id].do_send(&signupPacket);
			break;
		}
		else
			UserName.insert(p->userName);

		UserInfo.insert({ p->id,{p->password,p->userName} });
		signupPacket->success = true;
		clients[c_id].do_send(&signupPacket);
		break;
	}
	case CS_CHANGE_MAP: {
		CS_CHANGE_MAP_PACKET* p = reinterpret_cast<CS_CHANGE_MAP_PACKET*>(packet);
		for (auto& pl : clients) {
			if (false == pl.in_use) continue;

			SC_ADD_PLAYER_PACKET add_packet;
			add_packet.id = c_id;
			strcpy_s(add_packet.role, clients[c_id]._role);

			add_packet.size = sizeof(add_packet);
			add_packet.type = SC_ADD_PLAYER;
			add_packet.x = clients[c_id].x;
			add_packet.y = clients[c_id].y;
			add_packet.z = clients[c_id].z;
			add_packet._hp = clients[c_id]._hp;
			pl.do_send(&add_packet);
		}
		for (auto& pl : clients) {
			if (false == pl.in_use) continue;

			SC_ADD_PLAYER_PACKET add_packet;
			add_packet.id = pl._id;
			strcpy_s(add_packet.role, pl._role);
			add_packet.size = sizeof(add_packet);
			add_packet.type = SC_ADD_PLAYER;
			add_packet.x = pl.x;
			add_packet.y = pl.y;
			add_packet.z = pl.z;
			add_packet._hp = pl._hp;

			clients[c_id].do_send(&add_packet);
		}
		break;
	}
	case CS_MOVE: {
		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);

		clients[c_id].x = p->x;
		clients[c_id].y = p->y;
		clients[c_id].z = p->z;

		clients[c_id].rx = p->rx;
		clients[c_id].ry = p->ry;
		clients[c_id].rz = p->rz;

		clients[c_id].speed = p->speed;

		for (auto& pl : clients)
			if (true == pl.in_use)
				pl.send_move_packet(c_id);
		break;
	}
	case CS_ATTACK: {
		CS_ATTACK_PACKET* p = reinterpret_cast<CS_ATTACK_PACKET*>(packet);
		clients[c_id].x = p->x;
		clients[c_id].y = p->y;
		clients[c_id].z = p->z;

		//clients[c_id].ry = p->y;
		for (auto& pl : clients)
			if (true == pl.in_use)
				pl.send_attack_packet(c_id);
		break;

	}
	case CS_HITTED: {
		CS_HITTED_PACKET* p = reinterpret_cast<CS_HITTED_PACKET*>(packet);
		clients[c_id]._hp -= 50;

		if (clients[c_id]._hp <= 0) {
			for (auto& pl : clients)
				if (true == pl.in_use)
					pl.send_dead_packet(c_id);
			break;
		}
		for (auto& pl : clients)
			if (true == pl.in_use)
				pl.send_hitted_packet(c_id);
		break;
	}
	}
}

void disconnect(int c_id)
{
	for (auto& pl : clients) {
		if (pl.in_use == false) continue;
		if (pl._id == c_id) continue;
		SC_REMOVE_PLAYER_PACKET p;
		p.id = c_id;
		p.size = sizeof(p);
		p.type = SC_REMOVE_PLAYER;
		pl.do_send(&p);
	}
	closesocket(clients[c_id]._socket);
	clients[c_id].in_use = false;
}

int main()
{
	HANDLE h_iocp;

	srand((unsigned int)time(NULL));
	MAPID = rand() % 3 + 1;
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	SOCKET server = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(server, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(server, SOMAXCONN);
	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);
	int client_id = 0;

	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(server), h_iocp, 9999, 0);
	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	OVER_EXP a_over;
	a_over._comp_type = OP_ACCEPT;
	AcceptEx(server, c_socket, a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &a_over._over);

	while (true) {
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &over, INFINITE);
		OVER_EXP* ex_over = reinterpret_cast<OVER_EXP*>(over);
		if (FALSE == ret) {
			if (ex_over->_comp_type == OP_ACCEPT) cout << "Accept Error";
			else {
				if (CHANGEMAP) {
					MAPID = rand()%3 + 1;
					CHANGEMAP = false;
				}
				cout << "GQCS Error on client[" << key << "]\n";
				disconnect(static_cast<int>(key));
				if (ex_over->_comp_type == OP_SEND) delete ex_over;
				continue;
			}
		}

		switch (ex_over->_comp_type) {
		case OP_ACCEPT: {
			int client_id = get_new_client_id();
			if (client_id != -1) {
				clients[client_id].in_use = true;
				clients[client_id].x = 0;
				clients[client_id].y = 0;
				clients[client_id].z = 0;
				
				clients[client_id].rx = 0;
				clients[client_id].ry = 0;
				clients[client_id].rz = 0;


				clients[client_id]._id = client_id;
				clients[client_id]._role[0] = 0;
				clients[client_id]._prev_remain = 0;
				clients[client_id]._socket = c_socket;
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket), h_iocp, client_id, 0);
				clients[client_id].do_recv();
				c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
				cout << "Client [" << client_id << "] Login" << endl;
			}
			else {
				cout << "Max user exceeded.\n";
			}
			ZeroMemory(&a_over._over, sizeof(a_over._over));
			AcceptEx(server, c_socket, a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &a_over._over);
			break;
		}
		case OP_RECV: {
			int remain_data = num_bytes + clients[key]._prev_remain;
			char* p = ex_over->_send_buf;
			while (remain_data > 0) {
				int packet_size = p[0];
				if (packet_size <= remain_data) {
					process_packet(static_cast<int>(key), p);
					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else break;
			}
			clients[key]._prev_remain = remain_data;
			if (remain_data > 0) {
				memcpy(ex_over->_send_buf, p, remain_data);
			}
			clients[key].do_recv();
			break;
		}
		case OP_SEND:
			delete ex_over;
			break;
		}
	}
	closesocket(server);
	WSACleanup();
}
