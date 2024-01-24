#include "protocol.h"
#include "SESSION.h"
#include "OVER_EXP.h"
#include "Global.h"
#include "Fuse.h"
#include "stdafx.h"
#include "types.h"
#include "Json.h"
#include "FuseBox.h"
#include "Jelly.h"
#include "Portal.h"
#include "ItemBox.h"


map<std::string, array<std::string,2>> UserInfo;
set<std::string> UserName;
array <Fuse, MAX_FUSE_NUM> Fuses;
array <FuseBox, MAX_FUSE_BOX_NUM> FuseBoxes;
array<int, 8> FuseBoxList;
array<int, 8> FuseBoxColorList;
array<ItemBox, MAX_ITEM_BOX_NUM> ItemBoxes;
array<Jelly, MAX_JELLY_NUM> Jellys;
Portal portal;
mutex m;
bool InGame = false;

struct Timer{
	int		id;
	int		item;
	int		index;
	std::chrono::high_resolution_clock::time_point		current_time;
	std::chrono::high_resolution_clock::time_point		prev_time;
};
vector<Timer> TimerList;

void do_timer() {
	int i = 0;
	while (true) {
		i = 0;
		for (Timer t : TimerList) {
			if (clients[t.id].interaction == false) {
				TimerList.erase(TimerList.begin() + i);
				continue;
			}
			t.prev_time = t.current_time;
			t.current_time = std::chrono::high_resolution_clock::now();
			if (t.item == 1) {
				auto interaction_time = std::chrono::duration_cast<std::chrono::microseconds>(t.current_time - t.prev_time);
				ItemBoxes[t.index].progress += interaction_time.count() / (3.0 * SEC_TO_MICRO);
				if (ItemBoxes[t.index].progress >= 1) {
					ItemBoxes[t.index].gun = Gun(1);	// �ϴ� �� Ÿ�� 1�� ���� ���߿� �����Ұ�
					for (auto& pl : clients) {
						if (pl.in_use == true) {
							pl.send_item_box_opened_packet(t.index, ItemBoxes[t.index].gun.GetGunType());
						}
					}
					TimerList.erase(TimerList.begin() + i);
					i--;
				}
			}
			else if (t.item == 2) {
				auto interaction_time = std::chrono::duration_cast<std::chrono::microseconds>(t.current_time - t.prev_time);
				FuseBoxes[t.index].progress += interaction_time.count() / (30.0 * SEC_TO_MICRO);
				if (FuseBoxes[t.index].progress >= 1) {
					for (auto& pl : clients) {
						if (pl.in_use == true) {
							pl.send_fuse_box_opened_packet(t.index);
						}
					}
					TimerList.erase(TimerList.begin() + i);
					i--;
				}
			}
			i++;
		};
	}
}

// �� ũ�� = 126 x 126 x 73
// sector �� : 16 x 16 ( z���� ���� )
unordered_map<int, unordered_map<int, vector<Object>>> OBJS;		// �� ��ȣ , ���� , ��ü��

int MapId;

struct Vector2D {
	float x;
	float y;
};

typedef struct Rectangle {
	Vector2D center;
	float extentX;
	float extentY;
	float yaw;
}rectangle;

struct Circle {
	float x;
	float y;
	float z;
	float r;
};

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

bool AreCirecleAndSquareColliding(const Circle& circle, const rectangle& rect)
{
	float dx = circle.x - rect.center.x;
	float dy = circle.y - rect.center.y;
	float dist = sqrt(dx * dx + dy * dy);

	float max_sq = sqrt(rect.extentX * rect.extentX + rect.extentY * rect.extentY);

	if (dist > circle.r + max_sq)
		return false;
	return true;
}

void RenewColArea(int c_id, const Circle& circle)
{
	rectangle rec1;
	
	for (int x = 0; x < ceil(float(MAP_X) / COL_SECTOR_SIZE); ++x) {
		for (int y = 0; y < ceil(float(MAP_Y) / COL_SECTOR_SIZE); ++y) {
			rec1 = { {-(MAP_X / 2) + float(x) * 800 + 400,-(MAP_Y / 2) + float(y) * 800 + 400}, 400, 400, 0 };
			if (AreCirecleAndSquareColliding(circle, rec1)) {
				clients[c_id].ColArea.push_back(x + y * 16);
			}
		}
	}
}

bool ArePlayerColliding(const Circle& circle, const Object& obj)
{
	if (obj.in_use == false)
		return false;

	if (obj.pos_z - obj.extent_z > circle.z + circle.r)
		return false;

	if (obj.pos_z + obj.extent_z < circle.z - circle.r)
		return false;

	if (obj.type == 1) {
		float localX = (circle.x - obj.pos_x) * cos(-obj.yaw * M_PI / 180.0) -
			(circle.y - obj.pos_y) * sin(-obj.yaw * M_PI / 180.0);
		float localY = (circle.x - obj.pos_x) * sin(-obj.yaw * M_PI / 180.0) +
			(circle.y - obj.pos_y) * cos(-obj.yaw * M_PI / 180.0);

		// ���� ��ǥ�迡�� �浹 �˻�
		bool collisionX = std::abs(localX) <= obj.extent_x + circle.r;
		bool collisionY = std::abs(localY) <= obj.extent_y + circle.r;

		return collisionX && collisionY;
	}
}

bool CollisionTest(int c_id, float x, float y, float z, float r) {
	Circle circle;
	circle.x = x;
	circle.y = y;
	circle.z = z;
	circle.r = r;
	RenewColArea(c_id, circle);

	for (auto& colArea : clients[c_id].ColArea) {
		for (auto& colObject : OBJS[clients[c_id].map_id][colArea]) {
			if (ArePlayerColliding(circle, colObject)) {
				clients[c_id].ColArea.clear();
				return true;
			}
		}
	}
	clients[c_id].ColArea.clear();
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
		cout << "id: " << p->id << endl;
		cout << "pwd: " << p->password << endl;
		cout << "name: " << p->userName << endl;
		SC_SIGNUP_PACKET signupPacket;
		signupPacket.type = SC_SIGNUP;
		signupPacket.size = sizeof(SC_SIGNUP_PACKET);

		if (UserInfo.find(p->id) != UserInfo.end()) {	// �ߺ��Ǵ� ���̵� �ִ��� Ȯ��
			signupPacket.success = false;
			signupPacket.errorCode = 100;
			cout << "�̹� ������� ���̵� �Դϴ�.\n";
			clients[c_id].do_send(&signupPacket);
			break;
		}

		if (UserName.find(p->userName) != UserName.end()) {	// �ߺ��Ǵ� �г����� �ִ��� Ȯ��.
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
		if (strcmp(p->role, "Runner") == 0)
			clients[c_id].r = 10;
		if (strcmp(p->role, "Chaser") == 0)
			clients[c_id].r = 1;
		clients[c_id].charactorNum = p->charactorNum;
		clients[c_id]._ready = true;
		bool allPlayersReady = true; // ��� �÷��̾ �غ�?
		for (auto& pl : clients) {
			if (false == pl.in_use) continue;
			if (!pl._ready) {
				allPlayersReady = false;
				break;
			}
		}
		if (allPlayersReady) {
			MapId = rand() % 3 + 1;
			int patternid = rand() % 3 + 1;			//���� ����
			int colors[4]{ 0,0,0,0 };
			int pre = -1;
			int index;
			int pre_color[4]{ -1,-1,-1,-1 };
			for (int i = 0; i < 8; ++i) {
				for (;;) {
					index = rand() % 4;
					if (index == pre)
						continue;
					pre = index;
					break;
				}

				index += i / 2 * 4;
				FuseBoxList[i] = index;
				for (;;) {
					int color = rand() % 4;
					if (colors[color] == 2) {
						continue;
					}
					if (pre_color[color] == -1) {
						pre_color[color] = index;
					}
					else {
						FuseBoxes[pre_color[color]].matchIndex = index;
						FuseBoxes[index].matchIndex = pre_color[color];
					}
					FuseBoxColorList[i] = color;
					colors[color] += 1;
					FuseBoxes[index].color = color;
					break;
				}
			}
			portal.gauge = 0;
			SC_MAP_INFO_PACKET mapinfo_packet;
			mapinfo_packet.size = sizeof(mapinfo_packet);
			mapinfo_packet.type = SC_MAP_INFO;
			mapinfo_packet.mapid = 1;
			mapinfo_packet.patternid = patternid;
			for (int i = 0; i < 8; ++i) {
				mapinfo_packet.fusebox[i] = FuseBoxList[i];
				mapinfo_packet.fusebox_color[i] = FuseBoxColorList[i];
			}
			for (auto& pl : clients) {
				if (false == pl.in_use) continue;
				pl.do_send(&mapinfo_packet);
				pl.map_id = 1;
			}
		}
		cout << p->role << " \n";
		break;
	}

	case CS_MAP_LOADED: {
		if (InGame == false) {
			InGame = true;
			for (auto a : Fuses)
				a.SetStatus(AVAILABLE);
		}
		CS_MAP_LOADED_PACKET* p = reinterpret_cast<CS_MAP_LOADED_PACKET*>(packet);
		// add packet ���� 
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
			add_packet.charactorNum = clients[c_id].charactorNum;
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
			add_packet.charactorNum = pl.charactorNum;
			add_packet._hp = pl._hp;
			clients[c_id].do_send(&add_packet);
		}
		break;
	}
	case CS_MOVE: {
		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
		if (!CollisionTest(c_id, p->x, p->y, p->z, clients[c_id].r)) {
			clients[c_id].x = p->x;
			clients[c_id].y = p->y;
			clients[c_id].z = p->z;
		}
		else {
			static int num = 0;
			cout << c_id << " player in Wrong Place !" << num++ << endl;
		}

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
					std::cout << "�÷��̾ ������ �þ� �ۿ� �ֽ��ϴ�." << std::endl;
				}
			}
		}
		break;
	}
	case CS_PICKUP_FUSE: {
		if (strcmp(clients[c_id]._role, "Chaser") == 0)
			break;
		CS_PICKUP_FUSE_PACKET* p = reinterpret_cast<CS_PICKUP_FUSE_PACKET*>(packet);
		m.lock();
		if (Fuses[p->fuseIndex].GetStatus() == AVAILABLE) {
			Fuses[p->fuseIndex].SetStatus(ACQUIRED);
			m.unlock();
			clients[c_id].fuse = p->fuseIndex;
			for (auto& pl : clients) {
				if (true == pl.in_use) {
					pl.send_pickup_fuse_packet(c_id, p->fuseIndex);
				}
			}
		}
		else
			m.unlock();
		break;
	}

	case CS_PICKUP_GUN: {
		if (strcmp(clients[c_id]._role, "Chaser") == 0)
			break;

		CS_PICKUP_GUN_PACKET* p = reinterpret_cast<CS_PICKUP_GUN_PACKET*>(packet);

		if (clients[c_id].gun.GetGunType() == -1) {
			clients[c_id].gun.ChangeGunType(p->gunType);
			ItemBoxes[p->itemBoxIndex].gun.ChangeGunType(-1);
		}
		else {
			ItemBoxes[p->itemBoxIndex].gun.ChangeGunType(clients[c_id].gun.GetGunType());
			clients[c_id].gun.ChangeGunType(p->gunType);
		}

		for (auto& pl : clients) {
			if (true == pl.in_use) {
				pl.send_pickup_gun_packet(c_id, p->gunType, p->itemBoxIndex, ItemBoxes[p->itemBoxIndex].gun.GetGunType());
			}
		}

		break;
	}



	case CS_USE_GUN: {
		CS_USE_GUN_PACKET* p = reinterpret_cast<CS_USE_GUN_PACKET*>(packet);
		clients[c_id].gun.ChangeGunType(-1);
		for (auto& pl : clients) {
			if (true == pl.in_use) {
				pl.send_use_gun_packet(c_id);
			}
		}
		break;
	}

	case CS_PUT_FUSE: {
		if (clients[c_id].fuse == -1)
			break;
		CS_PUT_FUSE_PACKET* p = reinterpret_cast<CS_PUT_FUSE_PACKET*>(packet);
		clients[c_id].fuse = -1;
		FuseBoxes[p->fuseBoxIndex].active = true;
		for (auto& pl : clients) {
			if (pl.in_use == true) {
				pl.send_fuse_box_active_packet(p->fuseBoxIndex);
			}
		}
		if (FuseBoxes[FuseBoxes[p->fuseBoxIndex].matchIndex].active) {
			m.lock();
			if (portal.gauge == 0) {
				portal.gauge = 50;
				m.unlock();
				for (auto& pl : clients) {
					if (pl.in_use == true) {
						pl.send_half_portal_gauge_packet();
					}
				}
				break;
			}
			else if (portal.gauge == 50) {
				portal.gauge = 100;
				m.unlock();
				for (auto& pl : clients) {
					if (pl.in_use == true) {
						pl.send_max_portal_gauge_packet();
					}
				}
				break;
			}
			m.unlock();
			break;
		}
	}

	case CS_REMOVE_JELLY: {
		CS_REMOVE_JELLY_PACKET* p = reinterpret_cast<CS_REMOVE_JELLY_PACKET*>(packet);
		if (Jellys[p->jellyIndex].InUse()) {
			Jellys[p->jellyIndex].ChangeInUseState(false);
		}
		else
			break;

		for (auto& pl : clients) {
			if (pl.in_use == true) {
				pl.send_remove_jelly_packet(p->jellyIndex);
			}
		}

		break;
	}

	case CS_AIM_STATE: {
		for (auto& pl : clients) {
			if (pl.in_use == true) {
				pl.send_aim_state_packet(c_id);
			}
		}
		break;
	}

	case CS_IDLE_STATE: {
		for (auto& pl : clients) {
			if (pl.in_use == true) {
				pl.send_idle_state_packet(c_id);
			}
		}
		break;
	}
	case CS_PRESS_F: {
		CS_PRESS_F_PACKET* p = reinterpret_cast<CS_PRESS_F_PACKET*>(packet);
		if (p->item == 1) {
			if (ItemBoxes[p->index].interaction_id == -1) {
				ItemBoxes[p->index].interaction_id = c_id;
			}
			else if (ItemBoxes[p->index].interaction_id != c_id) {
				clients[c_id].send_not_interactive_packet();
				break;
			}
		}

		if (p->item == 2) {
			if (FuseBoxes[p->index].interaction_id == -1) {
				FuseBoxes[p->index].interaction_id = c_id;
			}
			else if (FuseBoxes[p->index].interaction_id != c_id) {
				clients[c_id].send_not_interactive_packet();
				break;
			}
		}
		clients[c_id].interaction = true;
		Timer timer;
		timer.id = c_id;
		timer.item = p->item;
		timer.index = p->index;
		timer.current_time = std::chrono::high_resolution_clock::now();
		TimerList.push_back(timer);
		if (p->item == 1) {
			for (auto& pl : clients) {
				if (pl.in_use == true) {
					pl.send_opening_item_box_packet(c_id, p->index, ItemBoxes[p->index].progress);
				}
			}
		}
		else if (p->item == 2) {
			for (auto& pl : clients) {
				if (pl.in_use == true) {
					pl.send_opening_fuse_box_packet(c_id, p->index, FuseBoxes[p->index].progress);
				}
			}
		}
		break;
	}

	case CS_RELEASE_F: {
		CS_RELEASE_F_PACKET* p = reinterpret_cast<CS_RELEASE_F_PACKET*>(packet);
		clients[c_id].interaction = false;
		int index = 0;
		if (p->item == 1) {
			clients[c_id].interaction = false;
			ItemBoxes[p->index].progress = 0;
			ItemBoxes[p->index].interaction_id = -1;
		}
		else if (p->item == 2) {
			clients[c_id].interaction = false;
			FuseBoxes[p->index].interaction_id = -1;
		}
		for (auto& pl : clients) {
			if (pl.in_use == true) {
				pl.send_stop_open_packet(c_id, p->item, p->index, ItemBoxes[p->index].progress);
			}
		}
		break;
	}
	default:
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



double dotProduct(const Vector2D& v1, const Vector2D& v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

// �浹ó���� ���� ����ü


void projectRectangleOntoAxis(const rectangle& rect, const Vector2D& axis, double& minProjection, double& maxProjection) {
	Vector2D vertices[4];
	// Define the vertices of the rectangle
	vertices[0] = { -rect.extentX, -rect.extentY };
	vertices[1] = { rect.extentX, -rect.extentY };
	vertices[2] = { rect.extentX, rect.extentY };
	vertices[3] = { -rect.extentX, rect.extentY };

	// Rotate the vertices based on yaw angle
	for (int i = 0; i < 4; ++i) {
		double rotatedX = vertices[i].x * cos(rect.yaw) - vertices[i].y * sin(rect.yaw);
		double rotatedY = vertices[i].x * sin(rect.yaw) + vertices[i].y * cos(rect.yaw);
		vertices[i].x = rotatedX + rect.center.x;
		vertices[i].y = rotatedY + rect.center.y;
	}

	// Project the vertices onto the axis and find the minimum and maximum projections
	minProjection = maxProjection = dotProduct(vertices[0], axis);
	for (int i = 1; i < 4; ++i) {
		double projection = dotProduct(vertices[i], axis);
		minProjection = min(minProjection, projection);
		maxProjection = max(maxProjection, projection);
	}
}

bool areRectanglesSeparated(const rectangle& rectangle1, const rectangle& rectangle2, const Vector2D& axis) {
	double minProjection1, maxProjection1, minProjection2, maxProjection2;

	projectRectangleOntoAxis(rectangle1, axis, minProjection1, maxProjection1);
	projectRectangleOntoAxis(rectangle2, axis, minProjection2, maxProjection2);

	return (maxProjection1 < minProjection2) || (maxProjection2 < minProjection1);
}

bool areRectanglesColliding(const rectangle& rectangle1, const rectangle& rectangle2) {
	// Check for separation along each axis
	Vector2D axes[] = { {1, 0}, {0, 1} };  // x�� y��
	for (const Vector2D& axis : axes) {
		if (areRectanglesSeparated(rectangle1, rectangle2, axis)) {
			return false;  // Separation found, no collision
		}
	}

	return true;  // No separation along any axis, collision detected
}

// �浹 ������ ���� ��ü���� �����ϴ� �Լ�
void add_colldata(Object obj) {
	rectangle rec1 ;
	rectangle rec2 = { {obj.pos_x, obj.pos_y}, obj.extent_x, obj.extent_y, obj.yaw * std::numbers::pi / 180.0 };
	for (int x = 0; x < ceil(float(MAP_X) / COL_SECTOR_SIZE); ++x) {
		for (int y = 0; y < ceil(float(MAP_Y) / COL_SECTOR_SIZE); ++y) {
			rec1 = { {-(MAP_X/2) + float(x) * 800 + 400,-(MAP_Y/ 2) +float(y) * 800 + 400}, 400, 400, 0 };
			if (areRectanglesColliding(rec1, rec2)) {
				OBJS[obj.map_num][x + y * 16].push_back(obj);
			}
		}
	}
}

// ��ü �ʱ�ȭ �Լ�
int InIt_Objects() {
	for (int mapNum = 1; mapNum < MAX_MAP_NUM + 1; ++mapNum) {
		char filePath[100];
		if (mapNum == 1)
			std::sprintf(filePath, "..\\..\\coll_data\\Stage%dCollision.json", mapNum);
		else if (mapNum == 2)
			std::sprintf(filePath, "..\\..\\coll_data\\Stage%dCollision.json", mapNum);
		else if (mapNum == 3)
			std::sprintf(filePath, "..\\..\\coll_data\\Stage%dCollision.json", mapNum);


		// ���� �б�
		ifstream file(filePath);
		if (!file.is_open()) {
			return 1;
		}

		// ���� ������ ���ڿ��� �б�
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string jsonString = buffer.str();

		// JSON �Ľ�
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
					object.in_use = true;
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
					object.map_num = mapNum;

					add_colldata(object);
					//OBJS[mapNum][i++] = object;
				}
			}

			// �����͸� ����غ���
			//for (const auto& pair : ST1_OBJS) {
			//	std::cout << "Key: " << pair.first << std::endl;
			//	std::cout << "  Type: " << pair.second.type << ", LocationX: " << pair.second.pos_x << ", LocationY: " << pair.second.pos_y << ", LocationZ: " << pair.second.pos_z << std::endl;
			//	// �ʿ��� ��ŭ �ٸ� ����� ���

			//}
		}
		else {
			std::cerr << "JSON parsing error." << std::endl;
		}
	}

	for (int mapNum = 1; mapNum < MAX_MAP_NUM + 1; ++mapNum) {
		char filePath[100];
		if (mapNum == 1)
			std::sprintf(filePath, "..\\..\\coll_data\\Stage%dFuseBoxCollision.json", mapNum);
		/*else if (mapNum == 2)
			std::sprintf(filePath, "..\\..\\coll_data\\Stage%dCollision.json", mapNum);
		else if (mapNum == 3)
			std::sprintf(filePath, "..\\..\\coll_data\\Stage%dCollision.json", mapNum);*/


			// ���� �б�
		ifstream file(filePath);
		if (!file.is_open()) {
			return 1;
		}

		// ���� ������ ���ڿ��� �б�
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string jsonString = buffer.str();

		// JSON �Ľ�
		const char* jsonData = jsonString.c_str();

		rapidjson::Document document;
		document.Parse(jsonData);

		int i = 0;
		if (!document.HasParseError()) {

			for (auto it = document.MemberBegin(); it != document.MemberEnd(); ++it) {
				FuseBox fuseBox;
				fuseBox.obj_name = it->name.GetString();
				const rapidjson::Value& dataArray = it->value;
				for (const auto& data : dataArray.GetArray()) {
					fuseBox.type = data["Type"].GetInt();
					fuseBox.pos_x = data["LocationX"].GetFloat();
					fuseBox.pos_y = data["LocationY"].GetFloat();
					fuseBox.pos_z = data["LocationZ"].GetFloat();
					fuseBox.extent_x = data["ExtentX"].GetFloat();
					fuseBox.extent_y = data["ExtentY"].GetFloat();
					fuseBox.extent_z = data["ExtentZ"].GetFloat();
					fuseBox.yaw = data["Yaw"].GetFloat();
					fuseBox.roll = data["Roll"].GetFloat();
					fuseBox.pitch = data["Pitch"].GetFloat();
					fuseBox.map_num = mapNum;
					FuseBoxes[data["index"].GetInt()] = fuseBox;
				}
			}

			// �����͸� ����غ���
			//for (const auto& pair : ST1_OBJS) {
			//	std::cout << "Key: " << pair.first << std::endl;
			//	std::cout << "  Type: " << pair.second.type << ", LocationX: " << pair.second.pos_x << ", LocationY: " << pair.second.pos_y << ", LocationZ: " << pair.second.pos_z << std::endl;
			//	// �ʿ��� ��ŭ �ٸ� ����� ���

			//}
		}
		else {
			std::cerr << "JSON parsing error." << std::endl;
		}

		for (int mapNum = 1; mapNum < MAX_MAP_NUM + 1; ++mapNum) {
			char filePath[100];
			if (mapNum == 1)
				std::sprintf(filePath, "..\\..\\coll_data\\Stage%dJelly.json", mapNum);
			/*else if (mapNum == 2)
				std::sprintf(filePath, "..\\..\\coll_data\\Stage%dCollision.json", mapNum);
			else if (mapNum == 3)
				std::sprintf(filePath, "..\\..\\coll_data\\Stage%dCollision.json", mapNum);*/


				// ���� �б�
			ifstream file(filePath);
			if (!file.is_open()) {
				return 1;
			}

			// ���� ������ ���ڿ��� �б�
			std::stringstream buffer;
			buffer << file.rdbuf();
			std::string jsonString = buffer.str();

			// JSON �Ľ�
			const char* jsonData = jsonString.c_str();

			rapidjson::Document document;
			document.Parse(jsonData);

			int i = 0;
			if (!document.HasParseError()) {

				for (auto it = document.MemberBegin(); it != document.MemberEnd(); ++it) {
					const rapidjson::Value& dataArray = it->value;
					for (const auto& data : dataArray.GetArray()) {
						Jelly jelly{ data["Type"].GetInt() ,
							data["LocationX"].GetFloat(),data["LocationY"].GetFloat(),data["LocationZ"].GetFloat(),
							data["ExtentX"].GetFloat(),data["ExtentY"].GetFloat(),data["ExtentZ"].GetFloat(),
							data["Yaw"].GetFloat(), data["Roll"].GetFloat(), data["Pitch"].GetFloat(),
							data["index"].GetInt()
						};
						Jellys[data["index"].GetInt()] = jelly;
					}
				}
			}
			else {
				std::cerr << "JSON parsing error." << std::endl;
			}
		}
		return 0;
	}
}

int main()
{
	// ���� �غ�
	cout << "�� ��ü�� �о������" << endl;
	if (InIt_Objects()) {
		cout << "�浹üũ ���� �о���� ����" << endl;
		return 1;
	}
	for (int i = 0; i < MAX_FUSE_BOX_NUM; ++i)
		FuseBoxes[i].index = i;
	cout << "�� ��ü �б� �Ϸ�" << endl;
	thread timerThread(do_timer);
	cout << "���� ����" << endl;
	// ���� ����
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
	timerThread.join();
}
