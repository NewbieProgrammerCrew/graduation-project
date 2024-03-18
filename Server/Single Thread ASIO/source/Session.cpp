#include "Session.h"

using namespace std;

extern unordered_map<int, unordered_map<int, vector<Object>>> OBJS;		
concurrency::concurrent_unordered_map<int, shared_ptr<cSession>> clients;
concurrency::concurrent_unordered_map<std::string, array<std::string, 2>> UserInfo;
concurrency::concurrent_unordered_set<std::string> UserName;
extern concurrency::concurrent_queue<int> AvailableUserIDs;
extern atomic_int NowUserNum;
extern array <FuseBox, MAX_FUSE_BOX_NUM> FuseBoxes;						

extern concurrency::concurrent_queue<int> AvailableRoomNumber;

concurrency::concurrent_queue<int> ChaserQueue;
concurrency::concurrent_queue<int> RunnerQueue;
concurrency::concurrent_unordered_map<int, IngameMapData> IngameMapDataList;  
concurrency::concurrent_unordered_map<int, cIngameData>	IngameDataList;

extern boost::asio::steady_timer timer;

struct Timer {
	int		id;
	int		item;
	int		index;
	std::chrono::high_resolution_clock::time_point		current_time;
	std::chrono::high_resolution_clock::time_point		prev_time;
};

vector<Timer> TimerList(100);

void DoTimer(const boost::system::error_code& error, boost::asio::steady_timer* pTimer)
{
	for (int i = 0; i < TimerList.size(); ++i) {
		Timer& t = TimerList[i];
		if (IngameDataList[t.id].GetDieState()) {
			if (!IngameDataList[t.id].GetInteractionState()) {
				TimerList.erase(TimerList.begin() + i);
				i--;
				continue;
			}
		}

		int room_num = t.id/5;
		t.prev_time = t.current_time;
		t.current_time = std::chrono::high_resolution_clock::now();
		if (t.item == 1) {
			if (IngameMapDataList[room_num]._ItemBoxes[t.index].interaction_id == -1) {
				TimerList.erase(TimerList.begin() + i);
				i--;
				continue;
			}
			auto interaction_time = std::chrono::duration_cast<std::chrono::microseconds>(t.current_time - t.prev_time);
			IngameMapDataList[room_num]._ItemBoxes[t.index].progress += interaction_time.count() / (3.0 * SEC_TO_MICRO);
			if (IngameMapDataList[room_num]._ItemBoxes[t.index].progress >= 1) {
				IngameMapDataList[room_num]._ItemBoxes[t.index].SetGunType(1);	// ÀÏ´Ü ÃÑ Å¸ÀÔ 1·Î °íÁ¤ ³ªÁß¿¡ ¼öÁ¤ÇÒ°Í

				for (int id : IngameMapDataList[room_num]._player_ids) {
					if (id == -1) continue;
					cout << "id : " << id << endl;
					clients[id]->Send_Item_Box_Opened_Packet(t.index, IngameMapDataList[room_num]._ItemBoxes[t.index].GetGunType());
				}
				TimerList.erase(TimerList.begin() + i);
				i--;
			}
		}
		else if (t.item == 2) {
			if (IngameMapDataList[room_num]._fuse_boxes[t.index]._interaction_id == -1) {
				TimerList.erase(TimerList.begin() + i);
				i--;
				continue;
			}
			auto interaction_time = std::chrono::duration_cast<std::chrono::microseconds>(t.current_time - t.prev_time);
			IngameMapDataList[room_num]._fuse_boxes[t.index]._progress += interaction_time.count() / (5.0 * SEC_TO_MICRO);
			//clients[t.id].fuseBoxProgress += interaction_time.count() / (5.0 * SEC_TO_MICRO); //[edit]
			if (IngameMapDataList[room_num]._fuse_boxes[t.index]._progress >= 1) {
				for (int id : IngameMapDataList[room_num]._player_ids) {
					if (id == -1) continue;
					cout << "id : " << id << endl;
					clients[id]->Send_Fuse_Box_Opened_Packet(t.index);
				}
				TimerList.erase(TimerList.begin() + i);
				i--;
			}
		}
		/*else if (t.item == -2) {
			auto interaction_time = std::chrono::duration_cast<std::chrono::microseconds>(t.current_time - t.prev_time);
			clients[t.id].resurrectionCooldown += interaction_time.count() / (25.0 * SEC_TO_MICRO);
			if (clients[t.id].resurrectionCooldown >= 1) {
				clients[t.id]._die = false;
				clients[t.id].resurrectionCooldown = 0;
				clients[t.id]._hp = clients[t.id].beforeHp + 400;
				clients[t.id].beforeHp += 400;
				clients[t.id].chaserDie = false;
				for (auto& pl : clients) {
					if (pl.in_use == true) {
						pl.send_chaser_resurrection_packet(t.id);
					}
				}
				TimerList.erase(TimerList.begin() + i);
				i--;
			}
		}*/
	};
	pTimer->expires_at(pTimer->expiry()+boost::asio::chrono::milliseconds(100));
	pTimer->async_wait(boost::bind(DoTimer, boost::asio::placeholders::error, pTimer));
}





struct Circle {
	float x;
	float y;
	float z;
	float r;
};
struct Vector2D {
	float x;
	float y;
};
struct Vector3D {
	float x, y, z;

	Vector3D(float x, float y, float z) : x(x), y(y), z(z) {}

	Vector3D operator-(const Vector3D& v) const {
		return Vector3D(x - v.x, y - v.y, z - v.z);
	}

	float dot(const Vector3D& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	float magnitude() const {
		return sqrt(x * x + y * y + z * z);
	}

	Vector3D normalize() const {
		float m = magnitude();
		return Vector3D(x / m, y / m, z / m);
	}
};
typedef struct Rectangle {
	Vector2D center;
	float extentX;
	float extentY;
	float yaw;
}rectangle;
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
				IngameDataList[c_id]._col_area.push_back(x + y * 16);
			}
		}
	}
}
bool ArePlayerColliding(const Circle& circle, const Object& obj)
{
	if (obj._in_use == false)
		return false;

	if (obj._pos_z - obj._extent_z > circle.z + circle.r)
		return false;

	if (obj._pos_z + obj._extent_z < circle.z - circle.r)
		return false;

	if (obj._type == 1) {
		float localX = (circle.x - obj._pos_x) * cos(-obj._yaw * PI / 180.0) -
			(circle.y - obj._pos_y) * sin(-obj._yaw * PI / 180.0);
		float localY = (circle.x - obj._pos_x) * sin(-obj._yaw * PI / 180.0) +
			(circle.y - obj._pos_y) * cos(-obj._yaw * PI / 180.0);

		
		bool collisionX = std::abs(localX) <= obj._extent_x + circle.r;
		bool collisionY = std::abs(localY) <= obj._extent_y + circle.r;

		return collisionX && collisionY;
	}
	return false;
}
bool CollisionTest(int c_id, float x, float y, float z, float r) {
	Circle circle;
	circle.x = x;
	circle.y = y;
	circle.z = z;
	circle.r = r;
	RenewColArea(c_id, circle);

	for (auto& colArea : IngameDataList[c_id]._col_area) {
		for (auto& colObject : OBJS[IngameMapDataList[IngameDataList[c_id].GetRoomNumber()]._map_num][colArea]) {
			if (ArePlayerColliding(circle, colObject)) {
				IngameDataList[c_id]._col_area.clear();
				return true;
			}
		}
	}
	IngameDataList[c_id]._col_area.clear();
	return false;
}
Vector3D yawToDirectionVector(float yawDegrees) {
	float yawRadians = yawDegrees * (PI / 180.0f);
	float x = cos(yawRadians);
	float y = sin(yawRadians);
	return Vector3D(x, y, 0);
}
float angleBetween(const Vector3D& v1, const Vector3D& v2) {
	float dotProduct = v1.dot(v2);
	float magnitudeProduct = v1.magnitude() * v2.magnitude();
	return acos(dotProduct / magnitudeProduct) * (180.0 / PI);  // Radians to degrees
}
void cSession::Send_Packet(void* packet, unsigned id)
{
	int packet_size = reinterpret_cast<unsigned char*>(packet)[0];
	unsigned char* buff = new unsigned char[packet_size];
	memcpy(buff, packet, packet_size);
	clients[id]->Do_Write(buff, packet_size);
}
void cSession::Process_Packet(unsigned char* packet, int c_id)
{
	switch (packet[1]) {
	case CS_LOGIN: {
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		if (UserInfo.find(p->id) == UserInfo.end()) {
			cout << "id is not equal\n";
			clients[c_id]->Send_Login_Fail_Packet();
			break;
		}
		else if (UserInfo[p->id][0] != p->password) {
			cout << "pwd is not equal\n";
			clients[c_id]->Send_Login_Fail_Packet();
			break;
		}
		clients[c_id]->Set_User_Name(UserInfo[p->id][1]);
		clients[c_id]->Send_Login_Info_Packet();
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
			clients[c_id]->Send_Packet(&signupPacket);
			break;
		}

		if (UserName.find(p->userName) != UserName.end()) {	
			signupPacket.success = false;
			signupPacket.errorCode = 101;
			clients[c_id]->Send_Packet(&signupPacket);
			break;
		}
		UserName.insert(p->userName);

		UserInfo[p->id] = { p->password, p->userName };
		signupPacket.success = true;
		signupPacket.errorCode = 0;
		clients[c_id]->Send_Packet(&signupPacket);
		break;
	}

	case CS_ROLE: {
		CS_ROLE_PACKET* p = reinterpret_cast<CS_ROLE_PACKET*>(packet);
		strcpy(clients[c_id]->_role, p->role);
		if (strcmp(p->role, "Runner") == 0){
			//clients[c_id].r = 10;
			RunnerQueue.push(c_id);

		}
		if (strcmp(p->role, "Chaser") == 0){
			//clients[c_id].r = 1;
			//ChaserID = c_id;
			ChaserQueue.push(c_id);
		}
		clients[c_id]->_charactor_num = p->charactorNum;
		clients[c_id]->_ready = true;

		cout << p->role << " \n";

		bool allPlayersReady = false; 
		if (ChaserQueue.unsafe_size() >= 1) {
			if (RunnerQueue.unsafe_size() >= 1) {			// [Edit]
				allPlayersReady = true;
			}
		}

		if (allPlayersReady) {
			IngameMapData igmd;
			if (!ChaserQueue.try_pop(igmd._player_ids[0])) break;
			/*for (int id : ExpiredPlayers) {
				if (id == igmd._player_ids[0]) {
					ExpiredPlayers.
				}
			}*/
			if (!RunnerQueue.try_pop(igmd._player_ids[1])) break;		// [Edit]

			if (clients[igmd._player_ids[1]]->_in_use == false) break;
			int mapId = rand() % 3 + 1;				
			int patternid = rand() % 3 + 1;			
			int colors[4]{ 0,0,0,0 };				
			int pre = -1;
			int index;
			int pre_color[4]{ -1,-1,-1,-1 };
			int fuseBoxColorList[8];

			for (int i = 0; i < 8; ++i) {
				for (;;) {
					index = rand() % 4;
					if (index == pre)
						continue;
					pre = index;
					break;
				}
			
				index += i / 2 * 4;
				igmd._fuse_box_list[i] = index;

				
				igmd._fuse_boxes[i]._extent_x = FuseBoxes[index]._extent_x;
				igmd._fuse_boxes[i]._extent_y = FuseBoxes[index]._extent_y;
				igmd._fuse_boxes[i]._extent_z = FuseBoxes[index]._extent_z;
				igmd._fuse_boxes[i]._pos_x = FuseBoxes[index]._pos_x;
				igmd._fuse_boxes[i]._pos_y = FuseBoxes[index]._pos_y;
				igmd._fuse_boxes[i]._pos_z = FuseBoxes[index]._pos_z;
				igmd._fuse_boxes[i]._yaw = FuseBoxes[index]._yaw;
				igmd._fuse_boxes[i]._roll = FuseBoxes[index]._roll;
				igmd._fuse_boxes[i]._pitch = FuseBoxes[index]._pitch;

				for (;;) {
					int color = rand() % 4;
					if (colors[color] == 2) {
						continue;
					}
					if (pre_color[color] == -1) {
						pre_color[color] = i;
					}
					else {
						igmd._fuse_boxes[pre_color[color]]._match_index = i;
						igmd._fuse_boxes[i]._match_index = pre_color[color];
					}
					fuseBoxColorList[i] = color;
					colors[color] += 1;
					igmd._fuse_boxes[i]._color = color;
					break;
				}
			}
			igmd._map_num = 1;								// [Edit]

			SC_MAP_INFO_PACKET mapinfo_packet;
			mapinfo_packet.size = sizeof(mapinfo_packet);
			mapinfo_packet.type = SC_MAP_INFO;
			mapinfo_packet.mapid = 1;						// [Edit]
			mapinfo_packet.patternid = patternid;
			for (int i = 0; i < 8; ++i) {
				mapinfo_packet.fusebox[i] = igmd._fuse_box_list[i];
				mapinfo_packet.fusebox_color[i] = fuseBoxColorList[i];
			}
			int roomNum;							
			AvailableRoomNumber.try_pop(roomNum);
			IngameMapDataList[roomNum] = igmd;
			for (int id : igmd._player_ids) {
				if (id == -1)
					continue;
				clients[id]->Send_Map_Info_Packet(mapinfo_packet);
				clients[id]->_room_num = roomNum;
			}
			
			// [Edit]
			{
				
				cIngameData data;
				data.SetRoomNumber(roomNum);
				data.SetPosition(-2874.972553, -3263.0, 100);
				data.SetRadian(1);
				data.SetHp(600);
				data.SetRole(clients[igmd._player_ids[0]]->_charactor_num);
				data.SetUserName(clients[igmd._player_ids[0]]->Get_User_Name());
				data.SetMyClientNumber(igmd._player_ids[0]);
				data.SetMyIngameNum(roomNum*5);
				IngameDataList[data.GetMyIngameNumber()] = data;
				clients[igmd._player_ids[0]]->Set_Ingame_Num(roomNum*5);

				cIngameData data2;
				data2.SetRoomNumber(roomNum);
				data2.SetPosition(-2427.765165, 2498.606435, 100);
				data2.SetRadian(10);
				data2.SetHp(600);
				data2.SetRole(clients[igmd._player_ids[1]]->_charactor_num);
				data2.SetUserName(clients[igmd._player_ids[1]]->Get_User_Name());
				data2.SetMyClientNumber(igmd._player_ids[1]);
				data2.SetMyIngameNum(roomNum*5 + 1);
				IngameDataList[data2.GetMyIngameNumber()] = data2;
				clients[igmd._player_ids[1]]->Set_Ingame_Num(roomNum*5+1);
			}
		}
		break;
	}

	case CS_MAP_LOADED: {
		clients[c_id]->_ingame = true;
		int roomNum = clients[c_id]->Get_Ingame_Num() / 5;
		IngameMapData igmd;
		igmd = IngameMapDataList[roomNum];
		
		CS_MAP_LOADED_PACKET* p = reinterpret_cast<CS_MAP_LOADED_PACKET*>(packet);
		bool allPlayersInMap = true; 
		for (int id : igmd._player_ids) {
			if (id == -1)
				break;
			if (!clients[id]->_ingame) {
				allPlayersInMap = false;
				break;
			}
		}
		if (!allPlayersInMap) break;

		
		cout << "map loaded\n";

		for (int m_id : igmd._player_ids) {
			if (m_id == -1)
				continue;

			for (int id : igmd._player_ids) {
				if (id == -1)
					continue;
				SC_ADD_PLAYER_PACKET app;
				app.size = sizeof(app);
				app.type = SC_ADD_PLAYER;
				app.id = clients[id]->Get_My_Id();
				strcpy_s(app.role, clients[id]->_role);
				app.x = IngameDataList[clients[id]->_ingame_num].GetPositionX();
				app.y = IngameDataList[clients[id]->_ingame_num].GetPositionY();
				app.z = IngameDataList[clients[id]->_ingame_num].GetPositionZ();
				app.charactorNum = IngameDataList[clients[id]->_ingame_num].GetRole();
				app._hp = IngameDataList[clients[id]->_ingame_num].GetHp();

				clients[m_id]->Send_Packet(&app);
			}
		}
		break;
	}

	case CS_MOVE: {
		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
		if (!CollisionTest(_ingame_num, p->x, p->y, p->z, IngameDataList[_ingame_num].GetRadian())) {
			IngameDataList[_ingame_num].SetPosition(p->x, p->y, p->z);
		}
		else {
			static int num = 0;
			cout << c_id << " player in Wrong Place !" << num++ << endl;
		}

		IngameDataList[_ingame_num].SetRotationValue(p->rx, p->ry, p->rz);
		IngameDataList[_ingame_num].SetSeppd(p->speed);
		IngameDataList[_ingame_num].SetJump(p->jump);

		for (int id : IngameMapDataList[_room_num]._player_ids){
			if (id == -1)
				continue;
			clients[id]->Send_Move_Packet(c_id);
		}
		break;
	}

	case CS_ATTACK: {		
		cout << "attack!!" << endl;
		CS_ATTACK_PACKET* p = reinterpret_cast<CS_ATTACK_PACKET*>(packet);
		IngameDataList[_ingame_num].SetPosition(p->x, p->y, p->z);
		IngameDataList[_ingame_num].SetRotationValue(p->rx, p->ry, p->rz);

		for (int id : IngameMapDataList[_room_num]._player_ids) {
			if (id == -1) continue;
			clients[id]->Send_Attack_Packet(c_id);
		}

		Vector3D seekerDir = yawToDirectionVector(p->ry);
		Vector3D seekerPos{ p->x,p->y,p->z };

		cout << seekerDir.x << " " << seekerDir.y << " " << seekerDir.z << endl;

		for (int i = 0; i < 5; ++i) {
			if (!IngameDataList[_room_num + i]._in_use) continue;
			if (_room_num + i == _ingame_num) continue;

			cIngameData igd = IngameDataList[_room_num + i];

			Vector3D playerPos{ igd.GetPositionX(),igd.GetPositionY(), igd.GetPositionZ() };
			Vector3D directionToPlayer = playerPos - seekerPos;
			if (IngameDataList[_ingame_num].GetRole() <= 5) {		
				const float MAX_SHOOTING_DISTANCE = 10000;  
				const float SHOOTING_ANGLE = 45.f;
				if (directionToPlayer.magnitude() > MAX_SHOOTING_DISTANCE) {
					continue;
				}
				float angle = angleBetween(seekerDir.normalize(), directionToPlayer.normalize());
				if (angle <= SHOOTING_ANGLE) {
					int victimId = IngameDataList[_room_num + i].GetMyClientNumber();
					IngameDataList[_ingame_num].ChangeDamagenIflictedOnEnemy(200);
					IngameDataList[_room_num + i].ChangeHp(-200);

					
					for (int id : IngameMapDataList[_room_num]._player_ids) {
						if (id == -1) continue;
						clients[id]->Send_Other_Player_Hitted_Packet(victimId, IngameDataList[_room_num + i].GetHp());
					}

					
					if (IngameDataList[_room_num + i].GetHp() <= 0) {
						for (int id : IngameMapDataList[_room_num]._player_ids) {
							if (id == -1) continue;
							clients[id]->Send_Other_Player_Dead_Packet(victimId);
						}
						IngameDataList[_room_num + i].SetDieState(true);
						break;
					}

				}
				else {
					std::cout << "not in my sight." << std::endl;
				}
			}
			else if (5 < IngameDataList[_ingame_num].GetRole()) {
				const float CHASING_ANGLE = 45.f;
				const float MAX_CHASING_DISTANCE = 70.f;
				if (directionToPlayer.magnitude() > MAX_CHASING_DISTANCE) {
					continue;
				}

				float angle = angleBetween(seekerDir.normalize(), directionToPlayer.normalize());
				if (angle <= CHASING_ANGLE) {
					int victimId = IngameDataList[_room_num + i].GetMyClientNumber();
					IngameDataList[_ingame_num].ChangeDamagenIflictedOnEnemy(200);
					IngameDataList[_room_num + i].ChangeHp(-200);

					
					for (int id : IngameMapDataList[_room_num]._player_ids) {
						if (id == -1) continue;
						clients[id]->Send_Other_Player_Hitted_Packet(victimId, IngameDataList[_room_num + i].GetHp());
					}

					
					if (IngameDataList[_room_num + i].GetHp() <= 0) {
						for (int id : IngameMapDataList[_room_num]._player_ids) {
							if (id == -1) continue;
							clients[id]->Send_Other_Player_Dead_Packet(victimId);
						}
						IngameDataList[_room_num + i].SetDieState(true);
						break;
					}
				}
				else {
					std::cout << "not in my sight." << std::endl;
				}
			}
		}
		break; 
	}

	case CS_PICKUP_FUSE: {
		if (5<(IngameDataList[_ingame_num].GetRole()))
			break;
		if (IngameDataList[_ingame_num].GetFuseIndex() != -1)
			break;
		CS_PICKUP_FUSE_PACKET* p = reinterpret_cast<CS_PICKUP_FUSE_PACKET*>(packet);
		if (IngameMapDataList[_room_num]._fuses[p->fuseIndex].GetStatus() == AVAILABLE) {
			IngameMapDataList[_room_num]._fuses[p->fuseIndex].SetStatus(ACQUIRED);
			IngameDataList[_ingame_num].SetFuseIndex(p->fuseIndex);
			for (int id : IngameMapDataList[_room_num]._player_ids) {
				if (id == -1)
					continue;
				clients[id]->Send_Pickup_Fuse_Packet(c_id, p->fuseIndex);
			}
		}
		break;
	}
	case CS_PRESS_F: {
		CS_PRESS_F_PACKET* p = reinterpret_cast<CS_PRESS_F_PACKET*>(packet);
		if (p->item == 1) {
			if (IngameMapDataList[_room_num]._ItemBoxes[p->index].interaction_id == -1) {
				IngameMapDataList[_room_num]._ItemBoxes[p->index].interaction_id = c_id;
			}
			else if (IngameMapDataList[_room_num]._ItemBoxes[p->index].interaction_id != c_id) {
				clients[c_id]->Send_Cannot_Interactive_Packet();
				break;
			}
		}

		if (p->item == 2) {
			if (IngameMapDataList[_room_num]._fuse_boxes[p->index]._interaction_id == -1) {
				IngameMapDataList[_room_num]._fuse_boxes[p->index]._interaction_id = c_id;
			}
			else if (IngameMapDataList[_room_num]._fuse_boxes[p->index]._interaction_id != c_id) {
				clients[c_id]->Send_Cannot_Interactive_Packet();
				break;
			}
		}
		IngameDataList[_ingame_num].SetInteractionState(true);
		Timer timer;
		timer.id = _ingame_num;
		timer.item = p->item;
		timer.index = p->index;
		timer.current_time = std::chrono::high_resolution_clock::now();
		TimerList.push_back(timer);
		cout << p->item << endl;
		if (p->item == 1) {
			for (int id : IngameMapDataList[_room_num]._player_ids) {
				if (id == -1) continue;
				clients[id]->Send_Item_Box_Opening_Packet(c_id, p->index, IngameMapDataList[_room_num]._ItemBoxes[p->index].progress);
			}
		}
		else if (p->item == 2) {
			for (int id : IngameMapDataList[_room_num]._player_ids) {
				if (id == -1) continue;
				clients[id]->Send_Fuse_Box_Opening_Packet(c_id, p->index, IngameMapDataList[_room_num]._fuse_boxes[p->index]._progress);
			}
		}
		break;
	}

	case CS_RELEASE_F: {
		CS_RELEASE_F_PACKET* p = reinterpret_cast<CS_RELEASE_F_PACKET*>(packet);
		if (!IngameDataList[_ingame_num].GetInteractionState()) {
			break;
		}
		IngameDataList[_ingame_num].SetInteractionState(false);
		int index = 0;
		if (p->item == 1) {
			IngameMapDataList[_room_num]._ItemBoxes[p->index].progress = 0;
			IngameMapDataList[_room_num]._ItemBoxes[p->index].interaction_id = -1;
			for (int id : IngameMapDataList[_room_num]._player_ids) {
				if (id == -1) continue;
				clients[id]->Send_Stop_Opening_Packet(c_id,p->item, p->index, IngameMapDataList[_room_num]._ItemBoxes[p->index].progress);
			}
		}
		else if (p->item == 2) {
			IngameMapDataList[_room_num]._fuse_boxes[p->index]._interaction_id = -1;
			for (int id : IngameMapDataList[_room_num]._player_ids) {
				if (id == -1) continue;
				clients[id]->Send_Stop_Opening_Packet(c_id,p->item, p->index, IngameMapDataList[_room_num]._fuse_boxes[p->index]._progress);
			}
		}

		break;
	}

	default: cout << "Invalid Packet From Client [" << c_id << "]  PacketID : " << int(packet[1]) << "\n"; //system("pause"); exit(-1);
	}
}
void cSession::Do_Read()
{
	auto self(shared_from_this());
	_socket.async_read_some(boost::asio::buffer(_data), [this, self](boost::system::error_code ec, std::size_t length) {
		if (ec)
		{
 			if (ec.value() == boost::asio::error::operation_aborted) return;
			cout << "Receive Error on Session[" << _my_id << "] ERROR_CODE[" << ec << "]\n";
			bool emptyRoom = true;
			// [Edit]
			int index = 0;
			for (int id : IngameMapDataList[_ingame_num/5]._player_ids) {
				if (id == _my_id) {
					IngameMapDataList[_ingame_num / 5]._player_ids[index] = -1;
					break;
				}
				else if (id != -1) {
					emptyRoom = false;
				}
				index++;
			}
			if (emptyRoom == true) {
				AvailableRoomNumber.push(_ingame_num / 5);
				IngameMapDataList.unsafe_erase(_ingame_num / 5);
			}
			IngameDataList.unsafe_erase(clients[_my_id]->Get_Ingame_Num());
			clients.unsafe_erase(_my_id);
			AvailableUserIDs.push(_my_id);
			NowUserNum--;
			return;
		}

		int dataToProcess = static_cast<int>(length);
		unsigned char* buf = _data;
		while (0 < dataToProcess) {
			if (0 == _curr_packet_size) {
				_curr_packet_size = buf[0];
				if (buf[0] > 255) {
					cout << "Invalid Packet Size [ << buf[0] << ]\n";
					exit(-1);
				}
			}
			int needToBuild = _curr_packet_size - _prev_data_size;
			if (needToBuild <= dataToProcess) {
				// ï¿½ï¿½Å¶ ï¿½ï¿½ï¿½ï¿½
				memcpy(_packet + _prev_data_size, buf, needToBuild);
				Process_Packet(_packet, _my_id);
				_curr_packet_size = 0;
				_prev_data_size = 0;
				dataToProcess -= needToBuild;
				buf += needToBuild;
			}
			else {
				memcpy(_packet + _prev_data_size, buf, dataToProcess);
				_prev_data_size += dataToProcess;
				dataToProcess = 0;
				buf += dataToProcess;
			}
		}
		Do_Read();
		});
}
void cSession::Do_Write(unsigned char* packet, std::size_t length)
{
	auto self(shared_from_this());
	_socket.async_write_some(boost::asio::buffer(packet, length), [this, self, packet, length](boost::system::error_code ec, std::size_t bytes_transferred) {
		if (!ec)
		{
			if (length != bytes_transferred) {
				cout << "Incomplete Send occured on Session[" << _my_id << "]. This Session should be closed.\n";
			}
			delete packet;
		}
		});
}
void cSession::Set_User_Name(std::string _user_name)
{
	_user_name = _user_name;
}
std::string cSession::Get_User_Name()
{
	return _user_name;
}
void cSession::Start()
{
	Do_Read();
}
void cSession::Send_Packet(void* packet)
{
	int packet_size = reinterpret_cast<unsigned char*>(packet)[0];
	unsigned char* buff = new unsigned char[packet_size];
	memcpy(buff, packet, packet_size);
	Do_Write(buff, packet_size);
}
int cSession::Get_My_Id()
{
	return _my_id;
}
void cSession::Send_Login_Fail_Packet()
{
	SC_LOGIN_FAIL_PACKET p;
	p.id = Get_My_Id();
	p.size = sizeof(SC_LOGIN_FAIL_PACKET);
	p.type = SC_LOGIN_FAIL;
	p.errorCode = 102;
	Send_Packet(&p);
}
void cSession::Send_Login_Info_Packet()
{
	SC_LOGIN_INFO_PACKET p;
	p.id = Get_My_Id();
	p.size = sizeof(SC_LOGIN_INFO_PACKET);
	p.type = SC_LOGIN_INFO;
	std::string user_name = Get_User_Name();
	strcpy(p.userName, user_name.c_str());

	Send_Packet(&p);
}
void cSession::Send_Map_Info_Packet(SC_MAP_INFO_PACKET p)
{
	Send_Packet(&p);
}
void cSession::Send_Move_Packet(int c_id)
{
	cIngameData igmd = IngameDataList[clients[c_id]->_ingame_num];
	SC_MOVE_PLAYER_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_MOVE_PLAYER_PACKET);
	p.type = SC_MOVE_PLAYER;
	p.x = igmd.GetPositionX();
	p.y = igmd.GetPositionY();
	p.z = igmd.GetPositionZ();
	p.rx = igmd.GetRotationValueX();
	p.ry = igmd.GetRotationValueY();
	p.rz = igmd.GetRotationValueZ();
	p.speed = igmd.GetSpeed();
	p.jump = igmd.GetJump();
	Send_Packet(&p);
}
void cSession::Send_Attack_Packet(int c_id)
{
	SC_ATTACK_PLAYER_PACKET p;
	p.size = sizeof(SC_ATTACK_PLAYER_PACKET);
	p.type = SC_ATTACK_PLAYER;
	p.id = c_id;
	Send_Packet(&p);
}
void cSession::Send_Other_Player_Hitted_Packet(int c_id, int hp)
{
	SC_HITTED_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_HITTED_PACKET);
	p.type = SC_HITTED;
	p._hp = hp;

	Send_Packet(&p);
}
void cSession::Send_Other_Player_Dead_Packet(int c_id)
{
	SC_DEAD_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_DEAD_PACKET);
	p.type = SC_DEAD;
	p._hp = 0;

	Send_Packet(&p);
}
void cSession::Send_Pickup_Fuse_Packet(int c_id, int index)
{
	SC_PICKUP_FUSE_PACKET p;
	p.size = sizeof(SC_PICKUP_FUSE_PACKET);
	p.type = SC_PICKUP_FUSE;
	p.index = index;
	p.id = c_id;
	Send_Packet(&p);
}
void cSession::Send_Cannot_Interactive_Packet() 
{
	SC_NOT_INTERACTIVE_PACKET p;
	p.size = sizeof(SC_NOT_INTERACTIVE_PACKET);
	p.type = SC_NOT_INTERACTIVE;
	Send_Packet(&p);
}
void cSession::Send_Item_Box_Opened_Packet(int index, int gun_type)
{
	SC_ITEM_BOX_OPENED_PACKET p;
	p.size = sizeof(SC_ITEM_BOX_OPENED_PACKET);
	p.type = SC_ITEM_BOX_OPENED;
	p.index = index;
	cout << index << endl;

	p.gun_id = gun_type;
	Send_Packet(&p);
}
void cSession::Send_Item_Box_Opening_Packet(int c_id, int index, float progress)
{
	SC_OPENING_ITEM_BOX_PACKET p;
	p.size = sizeof(SC_OPENING_ITEM_BOX_PACKET);
	p.type = SC_OPENING_ITEM_BOX;
	p.id = c_id;
	p.index = index;
	p.progress = progress;
	Send_Packet(&p);
}
void cSession::Send_Stop_Opening_Packet(int c_id, int item, int index, float progress)
{
	SC_STOP_OPENING_PACKET p;
	p.size = sizeof(SC_STOP_OPENING_PACKET);
	p.type = SC_STOP_OPENING;
	p.id = c_id;
	p.item = item;
	p.index = index;
	p.progress = progress;
	Send_Packet(&p);
}

void cSession::Send_Fuse_Box_Opening_Packet(int c_id, int index, float progress)
{
	SC_OPENING_FUSE_BOX_PACKET p;
	p.size = sizeof(SC_OPENING_FUSE_BOX_PACKET);
	p.type = SC_OPENING_FUSE_BOX;
	p.index = index;
	p.progress = progress;
	p.id = c_id;
	Send_Packet(&p);
}

void cSession::Send_Fuse_Box_Opened_Packet(int index)
{
	SC_FUSE_BOX_OPENED_PACKET p;
	p.size = sizeof(SC_FUSE_BOX_OPENED_PACKET);
	p.type = SC_FUSE_BOX_OPENED;
	p.index = index;
	Send_Packet(&p);
}


int cSession::Get_Ingame_Num()
{
	return _ingame_num;
}
void cSession::Set_Ingame_Num(int num)
{
	_ingame_num = num;
}