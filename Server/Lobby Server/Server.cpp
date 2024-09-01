#include "Global.h"
#include "Over_Exp.h"
#include "Session.h"
#include "lSession.h"

OVER_EXP g_a_over;
OVER_EXP l_a_over;
SOCKET g_s_socket, g_c_socket;
SOCKET l_s_socket, l_c_socket;
int Available_Ids = 0;

lSession lsession;
array<Session, MAX_USER> clients;
concurrency::concurrent_unordered_map<std::string, array<std::string, 2>> UserInfo;
concurrency::concurrent_unordered_set<std::string> UserName;
concurrency::concurrent_queue<int> RunnerQueue;
concurrency::concurrent_queue<int> ChaserQueue;


array<atomic<int>, 8>GameServerThreadContention;
array<int, 8>GameServerThreadRoomCount;
array<int, 8>GameServerPortNums;


int get_new_client_id()
{
	return Available_Ids++;
}

void process_packet(int c_id, char* packet)
{
	switch (packet[1]) {
	case CS_LOGIN: {
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		if (UserInfo.find(p->id) == UserInfo.end()) {
			cout << "id is not equal\n";
			clients[c_id].SendLoginFailPacket();
			break;
		}
		else if (UserInfo[p->id][0] != p->password) {
			cout << "pwd is not equal\n";
			clients[c_id].SendLoginFailPacket();
			break;
		}
		clients[c_id].user_name_ = UserInfo[p->id][1];
		clients[c_id].SendLoginInfoPacket();
		break;
	}

	case CS_SIGNUP: {
		CS_SIGNUP_PACKET* p = reinterpret_cast<CS_SIGNUP_PACKET*>(packet);
		cout << "id: " << p->id << endl;
		cout << "pwd: " << p->password << endl;
		cout << "name: " << p->userName << endl;
		SC_SIGNUP_PACKET signupPacket;
		signupPacket.type = SC_SIGNUP;
		signupPacket.size = sizeof(SC_SIGNUP_PACKET);

		if (UserInfo.find(p->id) != UserInfo.end()) {
			signupPacket.success = false;
			signupPacket.errorCode = 100;
			cout << "sign up fail.\n";
			clients[c_id].do_send(&signupPacket);
			break;
		}

		if (UserName.find(p->userName) != UserName.end()) {
			signupPacket.success = false;
			signupPacket.errorCode = 101;
			clients[c_id].do_send(&signupPacket);
			break;
		}
		UserName.insert(p->userName);

		UserInfo[p->id] = { p->password, p->userName };
		signupPacket.success = true;
		signupPacket.errorCode = 0;
		clients[c_id].do_send(&signupPacket);
		break;
	}

	case CS_ROLE: {
		CS_ROLE_PACKET* p = reinterpret_cast<CS_ROLE_PACKET*>(packet);
		strcpy_s(clients[c_id].role_, p->role);
		if (strcmp(p->role, "Runner") == 0) {
			//clients[c_id].r = 10;
			RunnerQueue.push(c_id);

		}
		if (strcmp(p->role, "Chaser") == 0) {
			//clients[c_id].r = 1;
			//ChaserID = c_id;
			ChaserQueue.push(c_id);
		}
		clients[c_id].charactor_num_ = p->charactorNum;
		clients[c_id].ready_ = true;

		cout << p->role << " \n";

		bool allPlayersReady = false;
		if (ChaserQueue.unsafe_size() >= MAX_CHASER_NUM) {
			if (RunnerQueue.unsafe_size() >= MAX_RUNNER_NUM) {
				allPlayersReady = true;
			}
		}

		if (allPlayersReady) {
			int chaser;
			if (!ChaserQueue.try_pop(chaser))
				break;
			if (clients[chaser].state_ == ST_FREE)
				break;
			int runners[MAX_RUNNER_NUM];
			for (int i = 0; i < MAX_RUNNER_NUM; ++i) {
				runners[i] = -1;
				if (!RunnerQueue.try_pop(runners[i])) {
					cout << "fail\n";
					for (int rn : runners) {
						if (rn == -1) {
							break;
						}
						RunnerQueue.push(rn);
					}
					ChaserQueue.push(chaser);
					return;
				}
				if (clients[runners[i]].state_ == ST_FREE) {
					cout << "fail\n";
					i--;
					continue;
				}
				cout << "success\n";
			}

			while (true) {
				int thread_index = 0;
				int i = 0;
				int min_thread_contention = 0x7FFFFFFF;
				for (int tc : GameServerThreadContention) {
					if (tc < min_thread_contention) {
						min_thread_contention = tc;
						thread_index = i;
					}
					i++;
				}
				if (GameServerThreadContention[thread_index].compare_exchange_weak(min_thread_contention, min_thread_contention + 1)) {
					int room_num = GameServerThreadRoomCount[thread_index]++;
					lsession.SendCreateRoomPacket(chaser, runners);
					cout << chaser << "\n";
					clients[chaser].SendGameStartPacket(GameServerPortNums[thread_index]);
					for (int rn : runners) {
						cout << rn << "\n";
						clients[rn].SendGameStartPacket(GameServerPortNums[thread_index]);
					}
					break;
				}
			}

		}
		break;
	}
	}
}
void process_l_packet(int c_id, char* packet)
{
	switch (packet[1]) {

	case GAME_SERVER_OPENED: {
		cout << "게임서버 준비 완료" << endl;
		lsession.SendConnectedPacket();
		break;
	}

	}
}

void disconnect(int c_id)
{
	for (auto& pl : clients) {
		{
			lock_guard<mutex> ll(pl.s_lock_);
			if (ST_INGAME != pl.state_) continue;
		}
		if (pl.id_ == c_id)continue;
	}
	closesocket(clients[c_id].socket_);

	lock_guard<mutex> ll(clients[c_id].s_lock_);
	clients[c_id].state_ = ST_FREE;
}

void worker_thread(HANDLE h_iocp)
{
	while (true) {
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &over, INFINITE);
		OVER_EXP* ex_over = reinterpret_cast<OVER_EXP*>(over);
		if (FALSE == ret) {
			if (ex_over->comp_type == OP_ACCEPT) cout << "Accept Error";
			else {
				cout << "GQCS Error on client[" << key << "]\n";
				disconnect(static_cast<int>(key));
				if (ex_over->comp_type == OP_SEND) delete ex_over;
				continue;
			}
		}

		if ((0 == num_bytes) && ((ex_over->comp_type == OP_RECV) || (ex_over->comp_type == OP_SEND))) {
			disconnect(static_cast<int>(key));
			if (ex_over->comp_type == OP_SEND) delete ex_over;
			continue;
		}

		switch (ex_over->comp_type) {
		case OP_ACCEPT: {
			if (static_cast<int>(key) == 9000) {
				lsession.socket_ = l_c_socket;
				lsession.id_ = 100'000;
				lsession.prev_remain_ = 0;
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(l_c_socket), h_iocp, 100'000, 0);
				lsession.do_recv();
				l_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
				ZeroMemory(&l_a_over.over, sizeof(l_a_over.over));
				int addr_size = sizeof(SOCKADDR_IN);
				AcceptEx(l_s_socket, l_c_socket, l_a_over.send_buf, 0, addr_size + 16, addr_size + 16, 0, &l_a_over.over);
			}
			else {
				int client_id = get_new_client_id();
				if (client_id != -1) {
					{
						lock_guard<mutex> ll(clients[client_id].s_lock_);
						clients[client_id].state_ = ST_ALLOC;
					}
					clients[client_id].id_ = client_id;
					clients[client_id].prev_remain_ = 0;
					clients[client_id].socket_ = g_c_socket;
					CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_c_socket),
						h_iocp, client_id, 0);
					clients[client_id].do_recv();
					g_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
				}
				else {
					cout << "Max user exceeded.\n";
				}
				ZeroMemory(&g_a_over.over, sizeof(g_a_over.over));
				int addr_size = sizeof(SOCKADDR_IN);
				AcceptEx(g_s_socket, g_c_socket, g_a_over.send_buf, 0, addr_size + 16, addr_size + 16, 0, &g_a_over.over);
			}
			break;
		}
		case OP_RECV: {
			if (static_cast<int>(key) == 100'000)
			{
				int remain_data = num_bytes + lsession.prev_remain_;
				char* p = ex_over->send_buf;
				while (remain_data > 0) {
					int packet_size = p[0];
					if (packet_size <= remain_data) {
						process_l_packet(static_cast<int>(key), p);
						p = p + packet_size;
						remain_data = remain_data - packet_size;
					}
					else break;
				}
				lsession.prev_remain_ = remain_data;
				if (remain_data > 0) {
					memcpy(ex_over->send_buf, p, remain_data);
				}
				lsession.do_recv();
			}
			else {
				int remain_data = num_bytes + clients[key].prev_remain_;
				char* p = ex_over->send_buf;
				while (remain_data > 0) {
					int packet_size = p[0];
					if (packet_size <= remain_data) {
						process_packet(static_cast<int>(key), p);
						p = p + packet_size;
						remain_data = remain_data - packet_size;
					}
					else break;
				}
				clients[key].prev_remain_ = remain_data;
				if (remain_data > 0) {
					memcpy(ex_over->send_buf, p, remain_data);
				}
				clients[key].do_recv();
			}
			break;
		}
		case OP_SEND:
			delete ex_over;
			break;
		}
	}
}

int main()
{
	HANDLE h_iocp;
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	for (int i = 0; i < 8; ++i) {
		GameServerPortNums[i] = 9200 + i;
	}
	cout << "게임서버 연결중" << endl;
	l_s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN l_server_addr;
	memset(&l_server_addr, 0, sizeof(l_server_addr));
	l_server_addr.sin_family = AF_INET;
	l_server_addr.sin_port = htons(LOBBY_SERVER);
	l_server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(l_s_socket, reinterpret_cast<sockaddr*>(&l_server_addr), sizeof(l_server_addr));
	listen(l_s_socket, SOMAXCONN);
	SOCKADDR_IN l_cl_addr;
	int l_addr_size = sizeof(l_cl_addr);
	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(l_s_socket), h_iocp, 9000, 0);
	l_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	l_a_over.comp_type = OP_ACCEPT;
	AcceptEx(l_s_socket, l_c_socket, l_a_over.send_buf, 0, l_addr_size + 16, l_addr_size + 16, 0, &l_a_over.over);
	cout << "로비서버 준비 중" << endl;
	cout << "로비서버 준비 완료" << endl;

	g_s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(g_s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(g_s_socket, SOMAXCONN);
	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_s_socket), h_iocp, 9999, 0);
	g_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	g_a_over.comp_type = OP_ACCEPT;
	AcceptEx(g_s_socket, g_c_socket, g_a_over.send_buf, 0, addr_size + 16, addr_size + 16, 0, &g_a_over.over);

	vector <thread> worker_threads;
	int num_threads = std::thread::hardware_concurrency();
	for (int i = 0; i < num_threads; ++i)
		worker_threads.emplace_back(worker_thread, h_iocp);
	for (auto& th : worker_threads)
		th.join();
	closesocket(g_s_socket);
	WSACleanup();
}
