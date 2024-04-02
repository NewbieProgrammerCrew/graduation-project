#include "Session.h"

using namespace std;

extern unordered_map<int, unordered_map<int, vector<Object>>> OBJS;		
extern array<Jelly, MAX_JELLY_NUM> Jellys;									// Á©¸® À§Ä¡ Á¤º¸
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

struct BombTimer {
	int		id;
	int		room_num;
	Bomb	bomb;
	std::chrono::high_resolution_clock::time_point		current_time;
	std::chrono::high_resolution_clock::time_point		prev_time;
};

struct Circle {
	double x;
	double y;
	double z;
	double r;
};
struct Vector2D {
	double x;
	double y;
};
typedef struct Rectangle {
	Vector2D center;
	double extentX;
	double extentY;
	double yaw;
}rectangle;


Vector3D parabolicMotion(const Vector3D& initialPosition, const Vector3D& initialVelocity, const Vector3D& acceleration, double time) {
	Vector3D halfAccel = acceleration * 0.5;
	Vector3D displacement = initialVelocity * time + halfAccel * (time * time);
	return initialPosition + displacement;
}

bool AreCirecleAndSquareColliding(const Circle& circle, const rectangle& rect)
{
	double dx = circle.x - rect.center.x;
	double dy = circle.y - rect.center.y;
	double dist = sqrt(dx * dx + dy * dy);

	double max_sq = sqrt(rect.extentX * rect.extentX + rect.extentY * rect.extentY);

	if (dist > circle.r + max_sq)
		return false;
	return true;
}
void RenewColArea(int c_id, const Circle& circle)
{
	rectangle rec1;

	for (int x = 0; x < ceil(double(MAP_X) / COL_SECTOR_SIZE); ++x) {
		for (int y = 0; y < ceil(double(MAP_Y) / COL_SECTOR_SIZE); ++y) {
			rec1 = { {-(MAP_X / 2) + double(x) * 800 + 400,-(MAP_Y / 2) + double(y) * 800 + 400}, 400, 400, 0 };
			if (AreCirecleAndSquareColliding(circle, rec1)) {
				IngameDataList[c_id].col_area_.push_back(x + y * 16);
			}
		}
	}
}
bool ArePlayerColliding(const Circle& circle, const Object& obj)
{
	if (obj.in_use_ == false)
		return false;

	if (obj.pos_z_ - obj.extent_z_ > circle.z + circle.r)
		return false;

	if (obj.pos_z_ + obj.extent_z_ < circle.z - circle.r)
		return false;

	if (obj.type_ == 1) {
		double localX = (circle.x - obj.pos_x_) * cos(-obj.yaw_ * PI / 180.0) -
			(circle.y - obj.pos_y_) * sin(-obj.yaw_ * PI / 180.0);
		double localY = (circle.x - obj.pos_x_) * sin(-obj.yaw_ * PI / 180.0) +
			(circle.y - obj.pos_y_) * cos(-obj.yaw_ * PI / 180.0);


		bool collisionX = std::abs(localX) <= obj.extent_x_ + circle.r;
		bool collisionY = std::abs(localY) <= obj.extent_y_ + circle.r;

		return collisionX && collisionY;
	}
	return false;
}
bool AreBombAndJellyColliding(const Circle& circle, const Jelly& jelly)
{
	if (jelly.in_use_ == false)
		return false;

	if (jelly.pos_z_ - jelly.extent_z_ > circle.z + circle.r)
		return false;

	if (jelly.pos_z_ + jelly.extent_z_ < circle.z - circle.r)
		return false;

	if (jelly.type_ == 1) {
		double localX = (circle.x - jelly.pos_x_) * cos(-jelly.yaw_ * PI / 180.0) -
			(circle.y - jelly.pos_y_) * sin(-jelly.yaw_ * PI / 180.0);
		double localY = (circle.x - jelly.pos_x_) * sin(-jelly.yaw_ * PI / 180.0) +
			(circle.y - jelly.pos_y_) * cos(-jelly.yaw_ * PI / 180.0);


		bool collisionX = std::abs(localX) <= jelly.extent_x_ + circle.r;
		bool collisionY = std::abs(localY) <= jelly.extent_y_ + circle.r;

		return collisionX && collisionY;
	}
	return false;
}

bool CollisionTest(int c_id, double x, double y, double z, double r) {
	Circle circle;
	circle.x = x;
	circle.y = y;
	circle.z = z;
	circle.r = r;
	RenewColArea(c_id, circle);

	for (auto& colArea : IngameDataList[c_id].col_area_) {
		for (auto& colObject : OBJS[IngameMapDataList[IngameDataList[c_id].room_num_].map_num_][colArea]) {
			if (ArePlayerColliding(circle, colObject)) {
				IngameDataList[c_id].col_area_.clear();
				return true;
			}
		}
	}
	IngameDataList[c_id].col_area_.clear();
	return false;
}
bool BombCollisionTest(int c_id, int room_num, double x, double y, double z, double r, int bomb_index) {
	Circle circle;
	circle.x = x;
	circle.y = y;
	circle.z = z;
	circle.r = r;

	vector<int> colAreas;

	rectangle rec1;

	for (int x = 0; x < ceil(double(MAP_X) / COL_SECTOR_SIZE); ++x) {
		for (int y = 0; y < ceil(double(MAP_Y) / COL_SECTOR_SIZE); ++y) {
			rec1 = { {-(MAP_X / 2) + double(x) * 800 + 400,-(MAP_Y / 2) + double(y) * 800 + 400}, 400, 400, 0 };
			if (AreCirecleAndSquareColliding(circle, rec1)) {
				colAreas.push_back(x + y * 16);
			}
		}
	}

	for (auto& jelly : Jellys) {
		if (AreBombAndJellyColliding(circle, jelly)) {
			for (int id : IngameMapDataList[room_num].player_ids_) {
				if (id == -1) continue;
				clients[id]->SendRemoveJellyPacket(jelly.index_);
				clients[id]->SendBombExplosionPacket(bomb_index);
			}
			return true;
		}
	}

	for (auto& colArea : colAreas) {
		for (auto& colObject : OBJS[IngameMapDataList[room_num].map_num_][colArea]) {
			if (ArePlayerColliding(circle, colObject)) {
				for (int id : IngameMapDataList[room_num].player_ids_) {
					if (id == -1) continue;
					clients[id]->SendBombExplosionPacket(bomb_index);
				}
				return true;
			}
		}
	}
	return false;
}

Vector3D yawToDirectionVector(double yawDegrees) {
	double yawRadians = yawDegrees * (PI / 180.0f);
	double x = cos(yawRadians);
	double y = sin(yawRadians);
	return Vector3D(x, y, 0);
}
double angleBetween(const Vector3D& v1, const Vector3D& v2) {
	double dotProduct = v1.dot(v2);
	double magnitudeProduct = v1.magnitude() * v2.magnitude();
	return acos(dotProduct / magnitudeProduct) * (180.0 / PI);  // Radians to degrees
}

queue<Timer> TimerQueue;
queue<BombTimer> BombTimerQueue;

void DoTimer(const boost::system::error_code& error, boost::asio::steady_timer* pTimer)
{
	for(int i = 0; i < TimerQueue.size(); ++i) {
		Timer& t = TimerQueue.front();

		if (IngameDataList[t.id].die_) {
			if(t.id % 5 != 0){
				TimerQueue.pop();
				continue;
			}
		}
		if (!IngameDataList[t.id].interaction_) {
			TimerQueue.pop();
			continue;
		}

		int room_num = t.id/5;

		t.prev_time = t.current_time;
		t.current_time = std::chrono::high_resolution_clock::now();

		if (t.item == 1) {
			if (IngameMapDataList[room_num].ItemBoxes_[t.index].interaction_id_ == -1) {
				TimerQueue.pop();
				continue;
			}
			auto interaction_time = std::chrono::duration_cast<std::chrono::microseconds>(t.current_time - t.prev_time);
			IngameMapDataList[room_num].ItemBoxes_[t.index].progress_ += interaction_time.count() / (3.0 * SEC_TO_MICRO);
			if (IngameMapDataList[room_num].ItemBoxes_[t.index].progress_ >= 1) {
				IngameMapDataList[room_num].ItemBoxes_[t.index].bomb_.bomb_type_ = 1;	// ÀÏ´Ü ÆøÅº Å¸ÀÔ 1·Î °íÁ¤ ³ªÁß¿¡ ¼öÁ¤ÇÒ°Í
				IngameMapDataList[room_num].ItemBoxes_[t.index].bomb_.index_ = t.index;
				for (int id : IngameMapDataList[room_num].player_ids_) {
					if (id == -1) continue;
					clients[id]->SendItemBoxOpenedPacket(t.index, IngameMapDataList[room_num].ItemBoxes_[t.index].bomb_.bomb_type_);
				}
				TimerQueue.pop();
				continue;
			}
		}
		else if (t.item == 2) {
			int serverFuseBoxIndex = IngameMapDataList[room_num].GetRealFuseBoxIndex(t.index);
			if (IngameMapDataList[room_num].fuse_boxes_[serverFuseBoxIndex].interaction_id_ == -1) {
				TimerQueue.pop();
				continue;
			}
			auto interaction_time = std::chrono::duration_cast<std::chrono::microseconds>(t.current_time - t.prev_time);
			IngameMapDataList[room_num].fuse_boxes_[serverFuseBoxIndex].progress_ += interaction_time.count() / (5.0 * SEC_TO_MICRO);
			//clients[t.id].fuseBoxprogress_ += interaction_time.count() / (5.0 * SEC_TO_MICRO); //[edit]
			if (IngameMapDataList[room_num].fuse_boxes_[serverFuseBoxIndex].progress_ >= 1) {
				for (int id : IngameMapDataList[room_num].player_ids_) {
					if (id == -1) continue;
					clients[id]->SendFuseBoxOpenedPacket(t.index);
				}
				TimerQueue.pop();
				continue;
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
				TimerQueue.erase(TimerQueue.begin() + i);
				i--;
			}
		}*/
		TimerQueue.pop();
		TimerQueue.push(t);
	};
	pTimer->expires_at(pTimer->expiry()+boost::asio::chrono::milliseconds(100));
	pTimer->async_wait(boost::bind(DoTimer, boost::asio::placeholders::error, pTimer));
}

void DoBombTimer(const boost::system::error_code& error, boost::asio::steady_timer* pTimer)
{
	Vector3D acceleration = { 0, 0, -9.8 };
	for (int i = 0; i < BombTimerQueue.size(); ++i) {
		BombTimer& t = BombTimerQueue.front();
		
		t.current_time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_diff = std::chrono::duration_cast<std::chrono::duration<double>>(t.current_time - t.prev_time);
		
		Vector3D position;
		position = parabolicMotion(t.bomb.pos_, t.bomb.initialVelocity_,acceleration, time_diff.count());
		BombTimerQueue.pop();
		if (!BombCollisionTest(t.id, t.room_num,position.x, position.y, position.z, t.bomb.r_, t.bomb.index_)) {
			BombTimerQueue.push(t);
		}
	};
	pTimer->expires_at(pTimer->expiry() + boost::asio::chrono::milliseconds(10));
	pTimer->async_wait(boost::bind(DoBombTimer, boost::asio::placeholders::error, pTimer));
}


void cSession::SendPacket(void* packet, unsigned id)
{
	int packet_size = reinterpret_cast<unsigned char*>(packet)[0];
	unsigned char* buff = new unsigned char[packet_size];
	memcpy(buff, packet, packet_size);
	clients[id]->DoWrite(buff, packet_size);
}
void cSession::ProcessPacket(unsigned char* packet, int c_id)
{
	switch (packet[1]) {
	case CS_LOGIN: {
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		if (UserInfo.find(p->id) == UserInfo.end()) {
			cout << "id is not equal\n";
			clients[c_id]->SendLoginFailPacket();
			break;
		}
		else if (UserInfo[p->id][0] != p->password) {
			cout << "pwd is not equal\n";
			clients[c_id]->SendLoginFailPacket();
			break;
		}
		clients[c_id]->user_name_ =  UserInfo[p->id][1];
		clients[c_id]->SendLoginInfoPacket();
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
			clients[c_id]->SendPacket(&signupPacket);
			break;
		}

		if (UserName.find(p->userName) != UserName.end()) {	
			signupPacket.success = false;
			signupPacket.errorCode = 101;
			clients[c_id]->SendPacket(&signupPacket);
			break;
		}
		UserName.insert(p->userName);

		UserInfo[p->id] = { p->password, p->userName };
		signupPacket.success = true;
		signupPacket.errorCode = 0;
		clients[c_id]->SendPacket(&signupPacket);
		break;
	}

	case CS_ROLE: {
		CS_ROLE_PACKET* p = reinterpret_cast<CS_ROLE_PACKET*>(packet);
		strcpy(clients[c_id]->role_, p->role);
		if (strcmp(p->role, "Runner") == 0) {
			//clients[c_id].r = 10;
			RunnerQueue.push(c_id);

		}
		if (strcmp(p->role, "Chaser") == 0) {
			//clients[c_id].r = 1;
			//ChaserID = c_id;
			ChaserQueue.push(c_id);
		}
		clients[c_id]->charactor_num_ = p->charactorNum;
		clients[c_id]->ready_ = true;

		cout << p->role << " \n";

		bool allPlayersReady = false;
		if (ChaserQueue.unsafe_size() >= 1) {
			if (RunnerQueue.unsafe_size() >= 1) {			// [Edit]
				allPlayersReady = true;
			}
		}

		if (allPlayersReady) {
			IngameMapData igmd;
			if (!ChaserQueue.try_pop(igmd.player_ids_[0])) break;
			/*for (int id : ExpiredPlayers) {
				if (id == igmd.player_ids_[0]) {
					ExpiredPlayers.
				}
			}*/
			if (!RunnerQueue.try_pop(igmd.player_ids_[1])) break;		// [Edit]

			if (clients[igmd.player_ids_[1]]->in_use_ == false) break;
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
				igmd.fuse_box_list_[i] = index;


				igmd.fuse_boxes_[i].extent_x_ = FuseBoxes[index].extent_x_;
				igmd.fuse_boxes_[i].extent_y_ = FuseBoxes[index].extent_y_;
				igmd.fuse_boxes_[i].extent_z_ = FuseBoxes[index].extent_z_;
				igmd.fuse_boxes_[i].pos_x_ = FuseBoxes[index].pos_x_;
				igmd.fuse_boxes_[i].pos_y_ = FuseBoxes[index].pos_y_;
				igmd.fuse_boxes_[i].pos_z_ = FuseBoxes[index].pos_z_;
				igmd.fuse_boxes_[i].yaw_ = FuseBoxes[index].yaw_;
				igmd.fuse_boxes_[i].roll_ = FuseBoxes[index].roll_;
				igmd.fuse_boxes_[i].pitch_ = FuseBoxes[index].pitch_;

				for (;;) {
					int color = rand() % 4;
					if (colors[color] == 2) {
						continue;
					}
					if (pre_color[color] == -1) {
						pre_color[color] = i;
					}
					else {
						igmd.fuse_boxes_[pre_color[color]].match_index_ = i;
						igmd.fuse_boxes_[i].match_index_ = pre_color[color];
					}
					fuseBoxColorList[i] = color;
					colors[color] += 1;
					igmd.fuse_boxes_[i].color_ = color;
					break;
				}
			}
			igmd.map_num_ = 1;								// [Edit]

			SC_MAP_INFO_PACKET mapinfo_packet;
			mapinfo_packet.size = sizeof(mapinfo_packet);
			mapinfo_packet.type = SC_MAP_INFO;
			mapinfo_packet.mapid = 1;						// [Edit]
			mapinfo_packet.patternid = patternid;
			for (int i = 0; i < 8; ++i) {
				mapinfo_packet.fusebox[i] = igmd.fuse_box_list_[i];
				mapinfo_packet.fusebox_color[i] = fuseBoxColorList[i];
			}
			int roomNum;
			AvailableRoomNumber.try_pop(roomNum);
			IngameMapDataList[roomNum] = igmd;
			for (int id : igmd.player_ids_) {
				if (id == -1)
					continue;
				clients[id]->SendMapInfoPacket(mapinfo_packet);
				clients[id]->room_num_ = roomNum;
			}

			// [Edit]
			{

				cIngameData data;
				data.room_num_ = roomNum;
				data.x_ = -2874.972553;
				data.y_ = -3263.0;
				data.z_ = 100;
				data.r_ = 1;
				data.hp_ = 600;
				data.role_ = clients[igmd.player_ids_[0]]->charactor_num_;
				data.user_name_ = clients[igmd.player_ids_[0]]->user_name_;
				data.my_client_num_ = igmd.player_ids_[0];
				data.my_ingame_num_ = roomNum * 5;
				IngameDataList[data.my_ingame_num_] = data;
				clients[igmd.player_ids_[0]]->ingame_num_ = roomNum * 5;

				cIngameData data2;
				data2.room_num_ = roomNum;
				data2.x_ = -2427.765165;
				data2.y_ = -2498.606435;
				data2.z_ = 100;
				data2.r_ = 10;
				data2.hp_ = 600;
				data2.role_ = clients[igmd.player_ids_[1]]->charactor_num_;
				data2.user_name_ = clients[igmd.player_ids_[1]]->user_name_;
				data2.my_client_num_ = igmd.player_ids_[1];
				data2.my_ingame_num_ = roomNum * 5 + 1;
				IngameDataList[data2.my_ingame_num_] = data2;
				clients[igmd.player_ids_[1]]->ingame_num_ = roomNum * 5 + 1;
			}
		}
		break;
	}

	case CS_MAP_LOADED: {
		ingame_ = true;
		int roomNum = clients[c_id]->ingame_num_ / 5;
		IngameMapData igmd;
		igmd = IngameMapDataList[roomNum];
		
		CS_MAP_LOADED_PACKET* p = reinterpret_cast<CS_MAP_LOADED_PACKET*>(packet);
		bool allPlayersInMap = true; 
		for (int id : igmd.player_ids_) {
			if (id == -1)
				break;
			if (!ingame_) {
				allPlayersInMap = false;
				break;
			}
		}
		if (!allPlayersInMap) break;

		
		cout << "map loaded\n";

		for (int m_id : igmd.player_ids_) {
			if (m_id == -1)
				continue;

			for (int id : igmd.player_ids_) {
				if (id == -1)
					continue;
				SC_ADD_PLAYER_PACKET app;
				app.size = sizeof(app);
				app.type = SC_ADD_PLAYER;
				app.id = clients[id]->my_id_;
				strcpy_s(app.role, clients[id]->role_);
				app.x = IngameDataList[clients[id]->ingame_num_].x_;
				app.y = IngameDataList[clients[id]->ingame_num_].y_;
				app.z = IngameDataList[clients[id]->ingame_num_].z_;
				app.charactorNum = IngameDataList[clients[id]->ingame_num_].role_;
				app._hp = IngameDataList[clients[id]->ingame_num_].hp_;

				clients[m_id]->SendPacket(&app);
			}
		}
		break;
	}

	case CS_MOVE: {
		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
		if (!CollisionTest(ingame_num_, p->x, p->y, p->z, IngameDataList[ingame_num_].r_)) {
			IngameDataList[ingame_num_].x_ = p->x;
			IngameDataList[ingame_num_].y_ = p->y;
			IngameDataList[ingame_num_].z_ = p->z;
		}
		else {
			static int num = 0;
			cout << c_id << " player in Wrong Place !" << num++ << endl;
		}

		IngameDataList[ingame_num_].rx_ = p->rx;
		IngameDataList[ingame_num_].ry_ = p->ry; 
		IngameDataList[ingame_num_].rz_ = p->rz; 
		IngameDataList[ingame_num_].pitch_ = p->pitch;
		IngameDataList[ingame_num_].speed_ = p->speed;
		IngameDataList[ingame_num_].jump_ = p->jump;

		for (int id : IngameMapDataList[room_num_].player_ids_){
			if (id == -1)
				continue;
			clients[id]->SendMovePacket(c_id);
		}
		break;
	}

	case CS_ATTACK: {		
		cout << "attack!!" << endl;
		CS_ATTACK_PACKET* p = reinterpret_cast<CS_ATTACK_PACKET*>(packet);
		IngameDataList[ingame_num_].x_ = p->x;
		IngameDataList[ingame_num_].y_ = p->y;
		IngameDataList[ingame_num_].z_ = p->z;
		IngameDataList[ingame_num_].rx_ = p->rx;
		IngameDataList[ingame_num_].ry_ = p->ry;
		IngameDataList[ingame_num_].rz_ = p->rz;

		for (int id : IngameMapDataList[room_num_].player_ids_) {
			if (id == -1) continue;
			clients[id]->SendAttackPacket(c_id);
		}

		Vector3D seekerDir = yawToDirectionVector(p->ry);
		Vector3D seekerPos{ p->x,p->y,p->z };

		cout << seekerDir.x << " " << seekerDir.y << " " << seekerDir.z << endl;

		for (int i = 0; i < 5; ++i) {				// RUNNER
			if (!IngameDataList[room_num_ + i].in_use_) continue;
			if (room_num_ + i == ingame_num_) continue;

			cIngameData igd = IngameDataList[room_num_ + i];

			Vector3D playerPos{ igd.x_,igd.y_, igd.z_ };
			Vector3D directionToPlayer = playerPos - seekerPos;
			if (IngameDataList[ingame_num_].role_ <= 5) {		
				const double MAX_SHOOTING_DISTANCE = 10000;  
				const double SHOOTING_ANGLE = 45.f;
				if (directionToPlayer.magnitude() > MAX_SHOOTING_DISTANCE) {
					continue;
				}
				double angle = angleBetween(seekerDir.normalize(), directionToPlayer.normalize());
				if (angle <= SHOOTING_ANGLE) {
					int victimId = IngameDataList[room_num_ + i].my_client_num_;
					IngameDataList[ingame_num_].damage_inflicted_on_enemy_ += 200;
					IngameDataList[room_num_ + i].hp_ -= 200;

					
					for (int id : IngameMapDataList[room_num_].player_ids_) {
						if (id == -1) continue;
						clients[id]->SendOtherPlayerHittedPacket(victimId, IngameDataList[room_num_ + i].hp_);
					}

					
					if (IngameDataList[room_num_ + i].hp_ <= 0) {
						for (int id : IngameMapDataList[room_num_].player_ids_) {
							if (id == -1) continue;
							clients[id]->SendOtherPlayerDeadPacket(victimId);
						}
						IngameDataList[room_num_ + i].die_ = true;
						break;
					}

				}
				else {
					std::cout << "not in my sight." << std::endl;
				}
			}
			else if (5 < IngameDataList[ingame_num_].role_) {			// CHASER
				const double CHASING_ANGLE = 45.f;
				const double MAX_CHASING_DISTANCE = 70.f;
				if (directionToPlayer.magnitude() > MAX_CHASING_DISTANCE) {
					continue;
				}

				double angle = angleBetween(seekerDir.normalize(), directionToPlayer.normalize());
				if (angle <= CHASING_ANGLE) {
					int victimId = IngameDataList[room_num_ + i].my_client_num_;
					IngameDataList[ingame_num_].damage_inflicted_on_enemy_+=200;
					IngameDataList[room_num_ + i].hp_ -=200;

					
					for (int id : IngameMapDataList[room_num_].player_ids_) {
						if (id == -1) continue;
						clients[id]->SendOtherPlayerHittedPacket(victimId, IngameDataList[room_num_ + i].hp_);
					}

					
					if (IngameDataList[room_num_ + i].hp_ <= 0) {
						for (int id : IngameMapDataList[room_num_].player_ids_) {
							if (id == -1) continue;
							clients[id]->SendOtherPlayerDeadPacket(victimId);
						}
						IngameDataList[room_num_ + i].die_ = true;
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
		if (5<(IngameDataList[ingame_num_].role_))
			break;
		if (IngameDataList[ingame_num_].fuse_ != -1)
			break;
		CS_PICKUP_FUSE_PACKET* p = reinterpret_cast<CS_PICKUP_FUSE_PACKET*>(packet);
		if (IngameMapDataList[room_num_].fuses_[p->fuseIndex].status_ == AVAILABLE) {
			IngameMapDataList[room_num_].fuses_[p->fuseIndex].status_ = ACQUIRED;
			IngameDataList[ingame_num_].fuse_ = p->fuseIndex;
			for (int id : IngameMapDataList[room_num_].player_ids_) {
				if (id == -1)
					continue;
				clients[id]->SendPickupFusePacket(c_id, p->fuseIndex);
			}
		}
		break;
	}
	case CS_PRESS_F: {
		CS_PRESS_F_PACKET* p = reinterpret_cast<CS_PRESS_F_PACKET*>(packet);
		int serverFuseBoxIndex = IngameMapDataList[room_num_].GetRealFuseBoxIndex(p->index);
		if (p->item == 1) {
			if (IngameMapDataList[room_num_].ItemBoxes_[p->index].interaction_id_ == -1) {
				IngameMapDataList[room_num_].ItemBoxes_[p->index].interaction_id_ = c_id;
			}
			else if (IngameMapDataList[room_num_].ItemBoxes_[p->index].interaction_id_ != c_id) {
				clients[c_id]->SendCannotInteractivePacket();
				break;
			}
		}
		else if (p->item == 2) {
			if (IngameMapDataList[room_num_].fuse_boxes_[serverFuseBoxIndex].interaction_id_ == -1) {
				IngameMapDataList[room_num_].fuse_boxes_[serverFuseBoxIndex].interaction_id_ = c_id;
			}
			else if (IngameMapDataList[room_num_].fuse_boxes_[serverFuseBoxIndex].interaction_id_ != c_id) {
				clients[c_id]->SendCannotInteractivePacket();
				break;
			}
		}
		IngameDataList[ingame_num_].interaction_ = true;

		Timer timer;
		timer.id = ingame_num_;
		timer.item = p->item;
		timer.index = p->index;
		timer.current_time = std::chrono::high_resolution_clock::now();
		TimerQueue.push(timer);
		cout << "press f" << endl;
		if (p->item == 1) {
			for (int id : IngameMapDataList[room_num_].player_ids_) {
				if (id == -1) continue;
				clients[id]->SendItemBoxOpeningPacket(c_id, p->index, IngameMapDataList[room_num_].ItemBoxes_[p->index].progress_);
			}
		}
		else if (p->item == 2) {
			for (int id : IngameMapDataList[room_num_].player_ids_) {
				if (id == -1) continue;
				clients[id]->SendFuseBoxOpeningPacket(c_id, p->index, IngameMapDataList[room_num_].fuse_boxes_[serverFuseBoxIndex].progress_);
			}
		}
		break;
	}

	case CS_RELEASE_F: {
		CS_RELEASE_F_PACKET* p = reinterpret_cast<CS_RELEASE_F_PACKET*>(packet);
		if (!IngameDataList[ingame_num_].interaction_) {
			break;
		}
		IngameDataList[ingame_num_].interaction_ = false;
		int index = 0;
		int serverFuseBoxIndex = IngameMapDataList[room_num_].GetRealFuseBoxIndex(p->index);

		if (p->item == 1) {
			IngameMapDataList[room_num_].ItemBoxes_[p->index].progress_ = 0;
			IngameMapDataList[room_num_].ItemBoxes_[p->index].interaction_id_ = -1;
			for (int id : IngameMapDataList[room_num_].player_ids_) {
				if (id == -1) continue;
				clients[id]->SendStopOpeningPacket(c_id,p->item, p->index, IngameMapDataList[room_num_].ItemBoxes_[p->index].progress_);
			}
		}
		else if (p->item == 2) {
			IngameMapDataList[room_num_].fuse_boxes_[serverFuseBoxIndex].interaction_id_ = -1;
			for (int id : IngameMapDataList[room_num_].player_ids_) {
				if (id == -1) continue;
				clients[id]->SendStopOpeningPacket(c_id,p->item, p->index, IngameMapDataList[room_num_].fuse_boxes_[serverFuseBoxIndex].progress_);
			}
		}
		cout << "release f" << endl;
		break;
	}

	case CS_PUT_FUSE: {
		if (IngameDataList[ingame_num_].fuse_ == -1)
			break;
		CS_PUT_FUSE_PACKET* p = reinterpret_cast<CS_PUT_FUSE_PACKET*>(packet);
		int serverFuseBoxIndex = IngameMapDataList[room_num_].GetRealFuseBoxIndex(p->fuseBoxIndex);
		IngameDataList[ingame_num_].fuse_ = -1;
		IngameMapDataList[room_num_].fuse_boxes_[serverFuseBoxIndex].active_ = true;
		for (int id : IngameMapDataList[room_num_].player_ids_) {
			if (id == -1) continue;
			clients[id]->SendFuseBoxActivePacket(p->fuseBoxIndex);
		}
		if (IngameMapDataList[room_num_].fuse_boxes_[IngameMapDataList[room_num_].fuse_boxes_[serverFuseBoxIndex].match_index_].active_) {
			if (IngameMapDataList[room_num_].portal_.gauge_ == 0) {
				IngameMapDataList[room_num_].portal_.gauge_ = 50;
				for (int id : IngameMapDataList[room_num_].player_ids_) {
					if (id == -1) continue;
					clients[id]->SendHalfPortalGaugePacket();
				}
				break;
			}
			else if (IngameMapDataList[room_num_].portal_.gauge_ == 50) {
				IngameMapDataList[room_num_].portal_.gauge_ = 100;
				for (int id : IngameMapDataList[room_num_].player_ids_) {
					if (id == -1) continue;
					clients[id]->SendMaxPortalGaugePacket();
				}
				break;
			}
		}
		break;
	}

	case CS_PICKUP_BOMB: {
		if (charactor_num_ > 5)
			break;
		CS_PICKUP_BOMB_PACKET* p = reinterpret_cast<CS_PICKUP_BOMB_PACKET*>(packet);
		int bomb_index = IngameMapDataList[room_num_].ItemBoxes_[p->itemBoxIndex].bomb_.index_;

		if (IngameDataList[ingame_num_].bomb_type_ == -1) {
			IngameDataList[ingame_num_].bomb_type_ = p->bombType;
			IngameDataList[ingame_num_].bomb_index_ = p->itemBoxIndex;
			IngameMapDataList[room_num_].ItemBoxes_[p->itemBoxIndex].bomb_.bomb_type_ = -1;
			IngameMapDataList[room_num_].ItemBoxes_[p->itemBoxIndex].bomb_.index_ = -1;
		}
		else {
			IngameMapDataList[room_num_].ItemBoxes_[p->itemBoxIndex].bomb_.bomb_type_ = IngameDataList[ingame_num_].bomb_type_;
			IngameMapDataList[room_num_].ItemBoxes_[p->itemBoxIndex].bomb_.index_ = IngameDataList[ingame_num_].bomb_index_;
			IngameDataList[ingame_num_].bomb_type_ = p->bombType;
			IngameDataList[ingame_num_].bomb_index_ = bomb_index;
		}

		for (int id : IngameMapDataList[room_num_].player_ids_) {
			if (id == -1) continue;
			clients[id]->SendPickUpBombPacket(c_id, p->bombType, p->itemBoxIndex, IngameMapDataList[room_num_].ItemBoxes_[p->itemBoxIndex].bomb_.bomb_type_, bomb_index);
		}
		break;
	}

	case CS_AIM_STATE: {
		for (int id : IngameMapDataList[room_num_].player_ids_) {
			if (id == -1) continue;
			clients[id]->SendAimStatePacket(c_id);
		}
		break;
	}
	case CS_IDLE_STATE: {
		for (int id : IngameMapDataList[room_num_].player_ids_) {
			if (id == -1) continue;
			clients[id]->SendIdleStatePacket(c_id);
		}
		break;
	}
	case CS_CANNON_FIRE: {
		CS_CANNON_FIRE_PACKET* p = reinterpret_cast<CS_CANNON_FIRE_PACKET*>(packet);

		Bomb bomb;
		bomb.pos_ = { p->x,p->y,p->z };
		bomb.initialVelocity_ = { p->rx, p->ry, p->rz };
		bomb.speed_ = BOMB_SPEED;
		bomb.initialVelocity_ = bomb.calculateInitialVelocity();
		bomb.bomb_type_ = IngameDataList[ingame_num_].bomb_type_;
		bomb.index_ = IngameDataList[ingame_num_].bomb_index_;

		BombTimer timer;
		timer.id = ingame_num_;
		timer.bomb = bomb;
		timer.room_num = room_num_;
		timer.prev_time = std::chrono::high_resolution_clock::now();
		BombTimerQueue.push(timer);

		IngameDataList[ingame_num_].bomb_type_ = -1;
		for (int id : IngameMapDataList[room_num_].player_ids_) {
			if (id == -1) continue;
			clients[id]->SendCannonFirePacket(c_id ,bomb);
		}
		break;
	}

	default: cout << "Invalid Packet From Client [" << c_id << "]  PacketID : " << int(packet[1]) << "\n"; //system("pause"); exit(-1);
	}
}
void cSession::DoRead()
{
	auto self(shared_from_this());
	socket_.async_read_some(boost::asio::buffer(data_), [this, self](boost::system::error_code ec, std::size_t length) {
		if (ec)
		{
 			if (ec.value() == boost::asio::error::operation_aborted) return;
			cout << "Receive Error on Session[" << my_id_ << "] ERROR_CODE[" << ec << "]\n";
			bool emptyRoom = true;
			// [Edit]
			int index = 0;
			for (int id : IngameMapDataList[ingame_num_/5].player_ids_) {
				if (id ==my_id_) {
					IngameMapDataList[ingame_num_ / 5].player_ids_[index] = -1;
					break;
				}
				else if (id != -1) {
					emptyRoom = false;
				}
				index++;
			}
			if (emptyRoom == true) {
				AvailableRoomNumber.push(ingame_num_ / 5);
				IngameMapDataList.unsafe_erase(ingame_num_ / 5);
			}
			IngameDataList.unsafe_erase(clients[my_id_]->ingame_num_);
			clients.unsafe_erase(my_id_);
			AvailableUserIDs.push(my_id_);
			NowUserNum--;
			return;
		}

		int dataToProcess = static_cast<int>(length);
		unsigned char* buf =data_;
		while (0 < dataToProcess) {
			if (0 == curr_packet_size_) {
				curr_packet_size_ = buf[0];
				if (buf[0] > 255) {
					cout << "Invalid Packet Size [ << buf[0] << ]\n";
					exit(-1);
				}
			}
			int needToBuild = curr_packet_size_ - prev_data_size_;
			if (needToBuild <= dataToProcess) {
				// ï¿½ï¿½Å¶ ï¿½ï¿½ï¿½ï¿½
				memcpy(packet_ + prev_data_size_, buf, needToBuild);
				ProcessPacket(packet_, my_id_);
				curr_packet_size_ = 0;
				prev_data_size_ = 0;
				dataToProcess -= needToBuild;
				buf += needToBuild;
			}
			else {
				memcpy(packet_ + prev_data_size_, buf, dataToProcess);
				prev_data_size_ += dataToProcess;
				dataToProcess = 0;
				buf += dataToProcess;
			}
		}
		DoRead();
		});
}
void cSession::DoWrite(unsigned char* packet, std::size_t length)
{
	auto self(shared_from_this());
	socket_.async_write_some(boost::asio::buffer(packet, length), [this, self, packet, length](boost::system::error_code ec, std::size_t bytes_transferred) {
		if (!ec)
		{
			if (length != bytes_transferred) {
				cout << "Incomplete Send occured on Session[" << my_id_ << "]. This Session should be closed.\n";
			}
			delete packet;
		}
		});
}

void cSession::Start()
{
	DoRead();
}
void cSession::SendPacket(void* packet)
{
	int packet_size = reinterpret_cast<unsigned char*>(packet)[0];
	unsigned char* buff = new unsigned char[packet_size];
	memcpy(buff, packet, packet_size);
	DoWrite(buff, packet_size);
}

void cSession::SendLoginFailPacket()
{
	SC_LOGIN_FAIL_PACKET p;
	p.id = my_id_;
	p.size = sizeof(SC_LOGIN_FAIL_PACKET);
	p.type = SC_LOGIN_FAIL;
	p.errorCode = 102;
	SendPacket(&p);
}
void cSession::SendLoginInfoPacket()
{
	SC_LOGIN_INFO_PACKET p;
	p.id = my_id_;
	p.size = sizeof(SC_LOGIN_INFO_PACKET);
	p.type = SC_LOGIN_INFO;
	std::string user_name = user_name_;
	strcpy(p.userName, user_name.c_str());

	SendPacket(&p);
}
void cSession::SendMapInfoPacket(SC_MAP_INFO_PACKET p)
{
	SendPacket(&p);
}
void cSession::SendMovePacket(int c_id)
{
	cIngameData igmd = IngameDataList[clients[c_id]->ingame_num_];
	SC_MOVE_PLAYER_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_MOVE_PLAYER_PACKET);
	p.type = SC_MOVE_PLAYER;
	p.x = igmd.x_;
	p.y = igmd.y_;
	p.z = igmd.z_;
	p.rx = igmd.rx_;
	p.ry = igmd.ry_;
	p.rz = igmd.rz_;
	p.pitch = igmd.pitch_;
	p.speed = igmd.speed_;
	p.jump = igmd.jump_;
	SendPacket(&p);
}
void cSession::SendAttackPacket(int c_id)
{
	SC_ATTACK_PLAYER_PACKET p;
	p.size = sizeof(SC_ATTACK_PLAYER_PACKET);
	p.type = SC_ATTACK_PLAYER;
	p.id = c_id;
	SendPacket(&p);
}
void cSession::SendOtherPlayerHittedPacket(int c_id, int hp)
{
	SC_HITTED_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_HITTED_PACKET);
	p.type = SC_HITTED;
	p._hp = hp;

	SendPacket(&p);
}
void cSession::SendOtherPlayerDeadPacket(int c_id)
{
	SC_DEAD_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_DEAD_PACKET);
	p.type = SC_DEAD;
	p._hp = 0;

	SendPacket(&p);
}
void cSession::SendPickupFusePacket(int c_id, int index)
{
	SC_PICKUP_FUSE_PACKET p;
	p.size = sizeof(SC_PICKUP_FUSE_PACKET);
	p.type = SC_PICKUP_FUSE;
	p.index = index;
	p.id = c_id;
	SendPacket(&p);
}
void cSession::SendCannotInteractivePacket() 
{
	SC_NOT_INTERACTIVE_PACKET p;
	p.size = sizeof(SC_NOT_INTERACTIVE_PACKET);
	p.type = SC_NOT_INTERACTIVE;
	SendPacket(&p);
}
void cSession::SendItemBoxOpenedPacket(int index, int bomb_type)
{
	SC_ITEM_BOX_OPENED_PACKET p;
	p.size = sizeof(SC_ITEM_BOX_OPENED_PACKET);
	p.type = SC_ITEM_BOX_OPENED;
	p.index = index;

	p.bomb_type = bomb_type;
	SendPacket(&p);
}
void cSession::SendItemBoxOpeningPacket(int c_id, int index, double progress)
{
	SC_OPENING_ITEM_BOX_PACKET p;
	p.size = sizeof(SC_OPENING_ITEM_BOX_PACKET);
	p.type = SC_OPENING_ITEM_BOX;
	p.id = c_id;
	p.index = index;
	p.progress = progress;
	SendPacket(&p);
}
void cSession::SendStopOpeningPacket(int c_id, int item, int index, double progress)
{
	SC_STOP_OPENING_PACKET p;
	p.size = sizeof(SC_STOP_OPENING_PACKET);
	p.type = SC_STOP_OPENING;
	p.id = c_id;
	p.item = item;
	p.index = index;
	p.progress = progress;
	SendPacket(&p);
}

void cSession::SendFuseBoxOpeningPacket(int c_id, int index, double progress)
{
	SC_OPENING_FUSE_BOX_PACKET p;
	p.size = sizeof(SC_OPENING_FUSE_BOX_PACKET);
	p.type = SC_OPENING_FUSE_BOX;
	p.index = index;
	p.progress = progress;
	p.id = c_id;
	SendPacket(&p);
}

void cSession::SendFuseBoxOpenedPacket(int index)
{
	SC_FUSE_BOX_OPENED_PACKET p;
	p.size = sizeof(SC_FUSE_BOX_OPENED_PACKET);
	p.type = SC_FUSE_BOX_OPENED;
	p.index = index;
	SendPacket(&p);
}

void cSession::SendFuseBoxActivePacket(int index)
{
	SC_FUSE_BOX_ACTIVE_PACKET p;
	p.size = sizeof(SC_FUSE_BOX_ACTIVE_PACKET);
	p.type = SC_FUSE_BOX_ACTIVE;
	p.fuseBoxIndex = index;
	SendPacket(&p);
}

void cSession::SendHalfPortalGaugePacket()
{
	SC_HALF_PORTAL_GAUGE_PACKET p;
	p.size = sizeof(SC_HALF_PORTAL_GAUGE_PACKET);
	p.type = SC_HALF_PORTAL_GAUGE;
	SendPacket(&p);
}

void cSession::SendMaxPortalGaugePacket()
{
	SC_MAX_PORTAL_GAUGE_PACKET p;
	p.size = sizeof(SC_MAX_PORTAL_GAUGE_PACKET);
	p.type = SC_MAX_PORTAL_GAUGE;
	SendPacket(&p);
}

void cSession::SendPickUpBombPacket(int c_id, int bomb_type, int item_box_index, int left_bomb_type, int bomb_index)
{
	SC_PICKUP_BOMB_PACKET p;
	p.size = sizeof(SC_PICKUP_BOMB_PACKET);
	p.type = SC_PICKUP_BOMB;
	p.id = c_id;
	p.bombType = bomb_type;
	p.itemBoxIndex = item_box_index;
	p.leftBombType = left_bomb_type;
	p.bombIndex = bomb_index;
	SendPacket(&p);
}

void cSession::SendAimStatePacket(int c_id)
{
	SC_AIM_STATE_PACKET p;
	p.size = sizeof(SC_AIM_STATE_PACKET);
	p.type = SC_AIM_STATE;
	p.id = c_id;
	SendPacket(&p);
}

void cSession::SendIdleStatePacket(int c_id)
{
	SC_IDLE_STATE_PACKET p;
	p.size = sizeof(SC_IDLE_STATE_PACKET);
	p.type = SC_IDLE_STATE;
	p.id = c_id;
	SendPacket(&p);
}

void cSession::SendCannonFirePacket(int c_id, Bomb bomb)
{
	SC_CANNON_FIRE_PACKET p;
	p.size = sizeof(SC_CANNON_FIRE_PACKET);
	p.type = SC_CANNON_FIRE;
	p.id = c_id;
	p.x = bomb.pos_.x;
	p.y = bomb.pos_.y;
	p.z = bomb.pos_.z;
	p.rx = bomb.rx_;
	p.ry = bomb.ry_;
	p.rz = bomb.rz_;
	p.bomb_type = bomb.bomb_type_;
	p.bomb_index = bomb.index_;
	SendPacket(&p);
}

void cSession::SendBombExplosionPacket(int index)
{
	SC_BOMB_EXPLOSION_PACKET p;
	p.size = sizeof(SC_BOMB_EXPLOSION_PACKET);
	p.type = SC_BOMB_EXPLOSION;
	p.bomb_index = index;
	SendPacket(&p);
}

void cSession::SendRemoveJellyPacket(int index)
{
	SC_REMOVE_JELLY_PACKET p;
	p.size = sizeof(SC_REMOVE_JELLY_PACKET);
	p.type = SC_REMOVE_JELLY;
	p.jellyIndex = index;
	SendPacket(&p);
}
