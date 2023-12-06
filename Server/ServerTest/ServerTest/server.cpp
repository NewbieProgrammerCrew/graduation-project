#include "protocol.h"
#include "SESSION.h"
#include "OVER_EXP.h"
#include "Global.h"
#include "Item.h"
#include "stdafx.h"
#include "types.h"
#include "Json.h"


map<std::string, array<std::string,2>> UserInfo;
set<std::string> UserName;
array<Item, MAX_ITEM> itemDatabase;
mutex m;

map<int,map<int, Object>> OBJS;

int MapId;


int get_new_client_id()
{
	for (int i = 0; i < MAX_USER; ++i)
		if (clients[i].in_use == false)
			return i;
	return -1;
}
float angleBetween(const Vector3D& v1, const Vector3D& v2) {
	float dotProduct = v1.dot(v2);
	float magnitudeProduct = v1.magnitude() * v2.magnitude();
	return acos(dotProduct / magnitudeProduct) * (180.0 / M_PI);  // Radians to degrees
}

Vector3D yawToDirectionVector(float yawDegrees) {
	float yawRadians = yawDegrees * (M_PI / 180.0f);
	float x = cos(yawRadians);
	float y = sin(yawRadians);
	return Vector3D(x, y, 0);
}

bool x_collision(float x, float y, float z)
{
	for (int i = 0; i < OBJS[MapId].size(); ++i) {
		if (OBJS[MapId][i].pos_x + OBJS[MapId][i].extent_x >= x)
			if (OBJS[MapId][i].pos_x - OBJS[MapId][i].extent_x <= x)
				return true;
	}
	return false;
}

bool y_collision(float x, float y, float z)
{
	for (int i = 0; i < OBJS[MapId].size(); ++i) {
		if (OBJS[MapId][i].pos_y + OBJS[MapId][i].extent_y >= x)
			if (OBJS[MapId][i].pos_y - OBJS[MapId][i].extent_y <= x)
				return true;
	}
	return false;
}

bool z_collision(float x, float y, float z)
{
	for (int i = 0; i < OBJS[MapId].size(); ++i) {
		if (OBJS[MapId][i].pos_z + OBJS[MapId][i].extent_z >= x)
			if (OBJS[MapId][i].pos_z - OBJS[MapId][i].extent_z <= x)
				return true;
	}
	return false;
}


void process_packet(int c_id, char* packet)
{
	switch (packet[1]) {
	case CS_LOGIN: {
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		if (!UserInfo.contains(p->id)) {
			cout << "id is not equal\n";
			clients[c_id].send_login_fail_packet();
			break;
		}
		else if (UserInfo[p->id][0] != p->password) {
			cout << "pwd is not equal\n";
			clients[c_id].send_login_fail_packet();
			break;
		}
		clients[c_id]._userName = UserInfo[p->id][1];
		clients[c_id].send_login_info_packet();
		break;
	}
	case CS_SIGNUP: {
		CS_SIGNUP_PACKET* p = reinterpret_cast<CS_SIGNUP_PACKET*>(packet);
		cout <<"id: " << p->id << endl;
		cout <<"pwd: " << p->password << endl;
		cout <<"name: " << p->userName << endl;
		SC_SIGNUP_PACKET signupPacket;
		signupPacket.type = SC_SIGNUP;
		signupPacket.size = sizeof(SC_SIGNUP_PACKET);

		if (UserInfo.find(p->id) != UserInfo.end()) {	// 중복되는 아이디가 있는지 확인
			signupPacket.success = false;
			signupPacket.errorCode = 100;
			cout << "이미 사용중인 아이디 입니다.\n";
			clients[c_id].do_send(&signupPacket);
			break;
		}

		if (UserName.find(p->userName)!= UserName.end()) {	// 중복되는 닉네임이 있는지 확인.
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
		strcpy(clients[c_id]._role, p->role);
		clients[c_id]._ready = true;
		bool allPlayersReady = true; // 모든 플레이어가 준비?
		for (auto& pl : clients) {
			if (false == pl.in_use) continue;
			if (!pl._ready){
				allPlayersReady = false;
				break;
			}
		}
		MapId = rand() % 3 + 1;
		int patternid = rand() % 3 + 1;			//패턴 정보
		if (allPlayersReady) {
			for (auto& pl : clients) {
				if (false == pl.in_use) continue;
				SC_MAP_INFO_PACKET mapinfo_packet;
				mapinfo_packet.size = sizeof(mapinfo_packet);
				mapinfo_packet.type = SC_MAP_INFO;
				mapinfo_packet.mapid = MapId;
				mapinfo_packet.patternid = patternid;
				pl.do_send(&mapinfo_packet);
			}
		}
		cout << p->role << " \n";
		break; 
	}
	
	case CS_MAP_LOADED: {
		CS_MAP_LOADED_PACKET* p = reinterpret_cast<CS_MAP_LOADED_PACKET*>(packet);
		// add packet 전송 
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
			if (strcmp(add_packet.role, "Runner") == 0) {
				clients[c_id]._hp = 300;
			}
			else if (strcmp(add_packet.role, "Chaser") == 0) {
				clients[c_id]._hp = 3000;
			}
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

		//if(!x_collision)
		clients[c_id].x = p->x;
		//if(!y_collision)
		clients[c_id].y = p->y;
		//if(!z_collision)
		clients[c_id].z = p->z;

		clients[c_id].rx = p->rx;
		clients[c_id].ry = p->ry;
		clients[c_id].rz = p->rz;

		clients[c_id].speed = p->speed;
		clients[c_id].jump = p->jump;

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

		for (auto& pl : clients)
			if (true == pl.in_use)
				pl.send_attack_packet(c_id);
		break;

	}
	case CS_HIT: {
		CS_HIT_PACKET* p = reinterpret_cast<CS_HIT_PACKET*>(packet);
		clients[c_id].x = p->x;
		clients[c_id].y = p->y;
		clients[c_id].z = p->z;
		clients[c_id].ry = p->ry;
	
		Vector3D seekerDir = yawToDirectionVector(p->ry);
		Vector3D seekerPos{ p->x,p->y,p->z };

		cout << seekerDir.x << " " << seekerDir.y << " " << seekerDir.z << endl;
		for (auto& pl : clients) {
			if (true == pl.in_use) {

				if (c_id == pl._id) {
					continue;
				}

				Vector3D playerPos{ pl.x, pl.y, pl.z };
				Vector3D directionToPlayer = playerPos - seekerPos;


				if (directionToPlayer.magnitude() > SOME_DISTANCE_THRESHOLD) {
					continue;
				}

				float angle = angleBetween(seekerDir.normalize(), directionToPlayer.normalize());
				if (angle <= 45.0) {
					pl._hp -= 50;

					cout << "CS_HIT!" << '\n';
					if (pl._hp <= 0) {
						for (auto& ppl : clients) {
							if (true == ppl.in_use && !ppl._die) {
								ppl.send_dead_packet(pl._id);
								ppl._die = true;
							}
						}
						break;
					}
					for (auto& ppl : clients)
						if (true == ppl.in_use)
							ppl.send_hitted_packet(pl._id);
				}
				else {
					std::cout << "플레이어가 술래의 시야 밖에 있습니다." << std::endl;
				}
			}
		}
		break;
	}
	case CS_PICKUP:
		if (strcmp(clients[c_id]._role, "Chaser") == 0)
			break;
		CS_ITEM_PICKUP_PACKET* p = reinterpret_cast<CS_ITEM_PICKUP_PACKET*>(packet);
		m.lock();
		if (itemDatabase[p->itemId].GetStatus() == AVAILABLE) {
			for (auto& pl : clients) {
				if (true == pl.in_use) {
					pl.send_pickup_packet(c_id);
				}
			}
		}
		itemDatabase[p->itemId].SetStatus(ACQUIRED);
		m.unlock();
		break;
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

int InIt_Objects() {
	for (int mapNum = 1; mapNum < MAX_MAP_NUM+1; ++mapNum) {
		char filePath[100];
		if (mapNum == 1)
			std::sprintf(filePath, "..\\..\\coll_data\\Stage%dCollision.json", mapNum);
		else if (mapNum == 2)
			std::sprintf(filePath, "..\\..\\coll_data\\Stage%dCollision.json", mapNum);
		else if (mapNum == 3)
			std::sprintf(filePath, "..\\..\\coll_data\\Stage%dCollision.json", mapNum);


		// 파일 읽기
		ifstream file(filePath);
		if (!file.is_open()) {
			return 1;
		}

		// 파일 내용을 문자열로 읽기
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string jsonString = buffer.str();

		// JSON 파싱
		const char* jsonData = jsonString.c_str();

		rapidjson::Document document;
		document.Parse(jsonData);

		int i = 0;
		if (!document.HasParseError()) {

			for (auto it = document.MemberBegin(); it != document.MemberEnd(); ++it) {
				Object object;
				object.obj_name = it->name.GetString();
				const rapidjson::Value& dataArray = it->value;
				for (const auto& data : dataArray.GetArray()) {
					object.type = data["Type"].GetInt();
					object.pos_x = data["LocationX"].GetFloat();
					object.pos_y = data["LocationY"].GetFloat();
					object.pos_z = data["LocationZ"].GetFloat();
					object.extent_x = data["ExtentX"].GetFloat();
					object.extent_y = data["ExtentY"].GetFloat();
					object.extent_z = data["ExtentZ"].GetFloat();
					object.yaw = data["Yaw"].GetFloat();
					object.roll = data["Roll"].GetFloat();
					object.pitch = data["Pitch"].GetFloat();

					OBJS[mapNum][i++] = object;
				}
			}

			// 데이터를 출력해보기
			//for (const auto& pair : ST1_OBJS) {
			//	std::cout << "Key: " << pair.first << std::endl;
			//	std::cout << "  Type: " << pair.second.type << ", LocationX: " << pair.second.pos_x << ", LocationY: " << pair.second.pos_y << ", LocationZ: " << pair.second.pos_z << std::endl;
			//	// 필요한 만큼 다른 멤버도 출력

			//}
		}
		else {
			std::cerr << "JSON parsing error." << std::endl;
		}
	}

	return 0;
}

int main()
{
	if (InIt_Objects()) {
		cout << "충돌체크 파일 읽어오기 실패" << endl;
		return 1;
	}
	HANDLE h_iocp;

	srand((unsigned int)time(NULL));
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
				clients[client_id]._die = false;
				clients[client_id].x = 0;
				clients[client_id].y = 0;
				clients[client_id].z = 0;
				
				clients[client_id].rx = 0;
				clients[client_id].ry = 0;
				clients[client_id].rz = 0;


				clients[client_id]._id = client_id;
				clients[client_id]._role[0] = 0;
				clients[client_id]._prev_remain = 0;
				clients[client_id]._ready = false;
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
