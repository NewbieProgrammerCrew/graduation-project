#include "Session.h"

using namespace std;

extern unordered_map<int, unordered_map<int, vector<Object>>> OBJS;		
extern array<Jelly, MAX_JELLY_NUM> Jellys;									// 젤리 위치 정보
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



enum TimerName{ItemBoxOpen, FuseBoxOpen, ChaserResurrection, ChaserHit};
struct Timer {
	int			id;
	TimerName status;
	int			index;
	std::chrono::high_resolution_clock::time_point		current_time;
	std::chrono::high_resolution_clock::time_point		prev_time;
};
BombType GetRandomBombType() {
	static std::random_device rd; 
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> distrib(0, Blind); 

	return static_cast<BombType>(distrib(gen));
}

struct BombTimer {
	int		id;
	int		room_num;
	Bomb	bomb;
	double	time_interval;
};

queue<Timer> TimerQueue;
queue<BombTimer> BombTimerQueue;


struct Vector2D {
	double x;
	double y;
	Vector2D operator-(const Vector2D& rhs) const {
		return { x - rhs.x, y - rhs.y };
	}

	// 벡터의 길이(크기) 계산
	double magnitude() const {
		return std::sqrt(x * x + y * y);
	}

	// 회전 변환
	Vector2D rotate(double yaw) const {
		double rad = yaw * M_PI / 180.0; // 각도를 라디안으로 변환
		double cos_rad = std::cos(rad);
		double sin_rad = std::sin(rad);
		if (std::abs(cos_rad) < std::numeric_limits<double>::epsilon())
			cos_rad = 0;
		if (std::abs(sin_rad) < std::numeric_limits<double>::epsilon())
			sin_rad = 0;
		return {
			y* sin_rad + x * cos_rad,
			y * cos_rad - x *sin_rad
		};
	}
};
struct Sphere {
	Vector2D center;
	double z;
	double r;
};

struct Circle {
	Vector2D center;
	double r;
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
bool AreCircleAndSquareColliding(const Circle& circle, const rectangle& rect)
{
	double dx = circle.center.x - rect.center.x;
	double dy = circle.center.y - rect.center.y;
	double dist = sqrt(dx * dx + dy * dy);

	double max_sq = sqrt(rect.extentX * rect.extentX + rect.extentY * rect.extentY);

	if (dist > circle.r + max_sq)
		return false;
	return true;
}
bool ArePlayerHitted(Circle& circle, rectangle& rect)
{
	Vector2D newCircle = {circle.center.x-rect.center.x, circle.center.y-rect.center.y};
	newCircle = newCircle.rotate(rect.yaw);
	double closestX = std::max(-rect.extentX+CHASER_HIT_RANGE, std::min(rect.extentX + CHASER_HIT_RANGE, newCircle.x));
	double closestY = std::max(-rect.extentY, std::min(rect.extentY, newCircle.y));
	Vector2D closestPoint = { closestX, closestY };
	double distance = (newCircle - closestPoint).magnitude();
	return distance <= circle.r;
}
bool AreCircleAndRectangleColliding(const Circle& circle, const rectangle& rect)
{
	Vector2D relativeCenter = rect.center - circle.center;
	Vector2D rotatedRelativeCenter = relativeCenter.rotate(-rect.yaw);
	double closestX = std::max(-rect.extentX, std::min(rect.extentX, rotatedRelativeCenter.x));
	double closestY = std::max(-rect.extentY, std::min(rect.extentY, rotatedRelativeCenter.y));
	Vector2D closestPoint = { closestX, closestY };

	double distance = (rotatedRelativeCenter - closestPoint).magnitude();

	return distance <= circle.r;
}

bool AreCircleAndCircleColliding(const Sphere& bomb, const Sphere& player, double player_extent_z) {
	if (bomb.z > player.z+ player_extent_z) 
		return false;
	if (bomb.z < player.z - player_extent_z)
		return false;

	float distance = sqrt(pow(player.center.x - bomb.center.x, 2) + pow(player.center.y - bomb.center.y, 2));
	if (distance <= (bomb.r + player.r)) {
		return true;
	}
	return false;
}
void RenewColArea(const int c_id, const Circle& circle)
{
	rectangle rec1;

	int minRow = max(0, ((static_cast<int>(circle.center.x)  + MAP_X/2 )/ COL_SECTOR_SIZE) - 1);
	int maxRow = min((static_cast<int>(circle.center.x) + MAP_X / 2) / COL_SECTOR_SIZE + 1, static_cast<int>(ceil(MAP_X / COL_SECTOR_SIZE)));
	int minCol = max(0, ((static_cast<int>(circle.center.y)+MAP_Y/2) / COL_SECTOR_SIZE) - 1);
	int maxCol = min((static_cast<int>(circle.center.y) + MAP_Y / 2) / COL_SECTOR_SIZE + 1, static_cast<int>(ceil(MAP_Y / COL_SECTOR_SIZE)));

	for (int row = minRow; row <= maxRow; ++row) {
		for (int col = minCol; col <= maxCol; ++col) {
			rec1 = { {-(MAP_X / 2) + double(row) * 800 + 400,-(MAP_Y / 2) + double(col) * 800 + 400}, 400, 400, 0 };
			if (AreCircleAndSquareColliding(circle, rec1)) {
				IngameDataList[c_id].col_area_.push_back(row + col * 16);
			}
		}
	}
}
bool ArePlayerColliding(const Sphere& sphere, const Object& obj)
{
	if (obj.in_use_ == false)
		return false;

	if (obj.pos_z_ - obj.extent_z_ > sphere.z + sphere.r)
		return false;

	if (obj.pos_z_ + obj.extent_z_ < sphere.z - sphere.r)
		return false;

	if (obj.type_ == 1) {
		double localX = (sphere.center.x - obj.pos_x_) * cos(-obj.yaw_ * M_PI / 180.0) -
			(sphere.center.y - obj.pos_y_) * sin(-obj.yaw_ * M_PI / 180.0);
		double localY = (sphere.center.x - obj.pos_x_) * sin(-obj.yaw_ * M_PI / 180.0) +
			(sphere.center.y - obj.pos_y_) * cos(-obj.yaw_ * M_PI / 180.0);


		bool collisionX = std::abs(localX) <= obj.extent_x_ + sphere.r;
		bool collisionY = std::abs(localY) <= obj.extent_y_ + sphere.r;

		return collisionX && collisionY;
	}
	return false;
}
bool AreBombAndJellyColliding(const Sphere& sphere, const Jelly& jelly)
{
	if (jelly.in_use_ == false)
		return false;

	if (jelly.pos_z_ - jelly.extent_z_ > sphere.z + sphere.r)
		return false;

	if (jelly.pos_z_ + jelly.extent_z_ < sphere.z - sphere.r)
		return false;

	if (jelly.type_ == 1) {
		double localX = (sphere.center.x - jelly.pos_x_) * cos(-jelly.yaw_ * M_PI / 180.0) -
			(sphere.center.y - jelly.pos_y_) * sin(-jelly.yaw_ * M_PI / 180.0);
		double localY = (sphere.center.x - jelly.pos_x_) * sin(-jelly.yaw_ * M_PI / 180.0) +
			(sphere.center.y - jelly.pos_y_) * cos(-jelly.yaw_ * M_PI / 180.0);


		bool collisionX = std::abs(localX) <= jelly.extent_x_ + sphere.r;
		bool collisionY = std::abs(localY) <= jelly.extent_y_ + sphere.r;

		return collisionX && collisionY;
	}
	return false;
}

bool CollisionTest(int c_id, double x, double y, double z, double r) {
	Sphere sphere;
	sphere.center = { x, y };
	sphere.z = z;
	sphere.r = r;
	Circle circle;
	circle.center = { x,y };
	circle.r = r;
	RenewColArea(c_id, circle);

	for (auto& colArea : IngameDataList[c_id].col_area_) {
		for (auto& colObject : OBJS[IngameMapDataList[IngameDataList[c_id].room_num_].map_num_][colArea]) {
			if (ArePlayerColliding(sphere, colObject)) {
				IngameDataList[c_id].col_area_.clear();
				return true;
			}
		}
	}
	IngameDataList[c_id].col_area_.clear();
	return false;
}

bool BombCollisionTest(const int c_id, const int room_num, const double x, const double y, const double z, const double r, const int bomb_index, const BombType bomb_type) {
	Sphere sphere;
	sphere.center = { x,y };
	sphere.z = z;
	sphere.r = r;
	
	Circle circle;
	circle.center = { x,y };
	circle.r = r;

	Sphere player;
	player.center.x = IngameDataList[room_num*5].x_;
	player.center.y = IngameDataList[room_num*5].y_;
	player.z = IngameDataList[room_num*5].z_;
	player.r = IngameDataList[room_num*5].r_;

	int chaserId = IngameMapDataList[room_num].player_ids_[0];

	vector<int> colAreas;

	int minRow = max(0, ((static_cast<int>(sphere.center.x) + MAP_X / 2) / COL_SECTOR_SIZE) - 1);
	int maxRow = min((static_cast<int>(sphere.center.x) + MAP_X / 2) / COL_SECTOR_SIZE + 1, static_cast<int>(ceil(MAP_X / COL_SECTOR_SIZE)));
	int minCol = max(0, ((static_cast<int>(sphere.center.y) + MAP_Y / 2) / COL_SECTOR_SIZE) - 1);
	int maxCol = min((static_cast<int>(sphere.center.y) + MAP_Y / 2) / COL_SECTOR_SIZE + 1, static_cast<int>(ceil(MAP_Y / COL_SECTOR_SIZE)));


	for (int row = minRow; row <= maxRow; ++row) {
		for (int col = minCol; col <= maxCol; ++col) {
			rectangle rec1 = { {-(MAP_X / 2) + double(row) * 800 + 400,-(MAP_Y / 2) + double(col) * 800 + 400}, 400, 400, 0 };
			if (AreCircleAndSquareColliding(circle, rec1)) {
				colAreas.push_back(row + col * 16);
			}
		}
	}

	for (auto& colArea : colAreas) {
		for (auto& colObject : OBJS[IngameMapDataList[room_num].map_num_][colArea]) {
			if (ArePlayerColliding(sphere, colObject)) {
				return true;
			}
		}
	}

	for (auto& jelly : Jellys) {
		if (AreBombAndJellyColliding(sphere, jelly)) {
			jelly.in_use_ = false;
			for (int id : IngameMapDataList[room_num].player_ids_) {
				if (id == -1) continue;
				clients[id]->SendRemoveJellyPacket(jelly.index_, bomb_index);
			}
			return true;
		}
	}

	if (AreCircleAndCircleColliding(sphere, player, IngameDataList[room_num*5].extent_z_)){
		if (bomb_type == Explosion) {
			IngameDataList[room_num*5].hp_ -= 200;
			if (IngameDataList[room_num*5].hp_ <= 0) {
				for (int id : IngameMapDataList[room_num].player_ids_) {
					if (id == -1) continue;
					clients[id]->SendOtherPlayerDeadPacket(chaserId);
				}
				Timer deadTimer;
				deadTimer.id = room_num*5;
				deadTimer.status = ChaserResurrection;
				deadTimer.current_time = std::chrono::high_resolution_clock::now();
				TimerQueue.push(deadTimer);
			}
			else {
				for (int id : IngameMapDataList[room_num].player_ids_) {
					if (id == -1) continue;
					clients[id]->SendOtherPlayerHittedPacket(chaserId, IngameDataList[room_num*5].hp_);
				}
			}
		}
		else if (bomb_type == Stun) {
			IngameDataList[room_num * 5].hp_ -= 100;
			if (IngameDataList[room_num * 5].hp_ <= 0) {
				for (int id : IngameMapDataList[room_num].player_ids_) {
					if (id == -1) continue;
					clients[id]->SendOtherPlayerDeadPacket(chaserId);
				}
				Timer deadTimer;
				deadTimer.id = room_num * 5;
				deadTimer.status = ChaserResurrection;
				deadTimer.current_time = std::chrono::high_resolution_clock::now();
				TimerQueue.push(deadTimer);
			}
			else {
				for (int id : IngameMapDataList[room_num].player_ids_) {
					if (id == -1) continue;
					clients[id]->SendOtherPlayerHittedPacket(chaserId, IngameDataList[room_num * 5].hp_);
				}
			}
		}
		else if (bomb_type == Blind) {
			IngameDataList[room_num * 5].hp_ -= 50;
			if (IngameDataList[room_num * 5].hp_ <= 0) {
				for (int id : IngameMapDataList[room_num].player_ids_) {
					if (id == -1) continue;
					clients[id]->SendOtherPlayerDeadPacket(chaserId);
				}
				Timer deadTimer;
				deadTimer.id = room_num * 5;
				deadTimer.status = ChaserResurrection;
				deadTimer.current_time = std::chrono::high_resolution_clock::now();
				TimerQueue.push(deadTimer);
			}
			else {
				for (int id : IngameMapDataList[room_num].player_ids_) {
					if (id == -1) continue;
					clients[id]->SendOtherPlayerHittedPacket(chaserId, IngameDataList[room_num * 5].hp_);
				}
			}
		}
		return true;
	}

	return false;
}

Vector3D yawToDirectionVector(double yawDegrees) {
	double yawRadians = yawDegrees * (M_PI / 180.0f);
	double x = cos(yawRadians);
	double y = sin(yawRadians);
	return Vector3D(x, y, 0);
}

double angleBetween(const Vector3D& v1, const Vector3D& v2) {
	double dotProduct = v1.dot(v2);
	double magnitudeProduct = v1.magnitude() * v2.magnitude();
	return acos(dotProduct / magnitudeProduct) * (180.0 / M_PI);  // Radians to degrees
}

void DoTimer(const boost::system::error_code& error, boost::asio::steady_timer* pTimer)
{
	for(int i = 0; i < TimerQueue.size(); ++i) {
		Timer& t = TimerQueue.front();

		if (IngameDataList[t.id].die_) {
			if(t.id % 5 != 0){
				TimerQueue.pop();
				continue;
			}
			else {
				if (!IngameDataList[t.id].interaction_) {
					TimerQueue.pop();
					continue;
				}
			}
		}
		

		int room_num = t.id/5;

		t.prev_time = t.current_time;
		t.current_time = std::chrono::high_resolution_clock::now();
		switch (t.status) {
		case ItemBoxOpen: {
			if (IngameMapDataList[room_num].ItemBoxes_[t.index].interaction_id_ == -1) {
				TimerQueue.pop();
				continue;
			}
			auto interaction_time = std::chrono::duration_cast<std::chrono::microseconds>(t.current_time - t.prev_time);
			IngameMapDataList[room_num].ItemBoxes_[t.index].progress_ += interaction_time.count() / (3.0 * SEC_TO_MICRO);
			if (IngameMapDataList[room_num].ItemBoxes_[t.index].progress_ >= 1) {
				IngameMapDataList[room_num].ItemBoxes_[t.index].bomb_.bomb_type_ = GetRandomBombType();
				IngameMapDataList[room_num].ItemBoxes_[t.index].bomb_.index_ = t.index;
				for (int id : IngameMapDataList[room_num].player_ids_) {
					if (id == -1) continue;
					clients[id]->SendItemBoxOpenedPacket(t.index, IngameMapDataList[room_num].ItemBoxes_[t.index].bomb_.bomb_type_);
				}
				TimerQueue.pop();
				continue;
			}
			break;
		}
		case FuseBoxOpen: {
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
			break;
		}
		case ChaserResurrection: {
			auto interaction_time = std::chrono::duration_cast<std::chrono::microseconds>(t.current_time - t.prev_time);
			IngameDataList[t.id].resurrectionCooldown_ -= (float(interaction_time.count()) / SEC_TO_MICRO);
			if (IngameDataList[t.id].resurrectionCooldown_ <= 0) {
				IngameDataList[t.id].die_ = false;
				IngameDataList[t.id].resurrectionCount += 1;
				IngameDataList[t.id].resurrectionCooldown_ = 10 * IngameDataList[t.id].resurrectionCount;
				IngameDataList[t.id].hp_ = IngameDataList[t.id].before_hp_ + 400;
				IngameDataList[t.id].before_hp_ += 400;
				for (int id : IngameMapDataList[room_num].player_ids_) {
					if (id == -1) continue;
					clients[id]->SendChaserResurrectionPacket(id / 5 * 5);
				}
				TimerQueue.pop();
				continue;
			}
			break;
		}
		case ChaserHit: {
			auto timeDiff = std::chrono::high_resolution_clock::now() - t.prev_time;
			if (std::chrono::duration_cast<std::chrono::seconds>(timeDiff).count() < 1)
				break;
			rectangle attackRange;
			attackRange.center.x = IngameDataList[t.id].x_;
			attackRange.center.y = IngameDataList[t.id].y_;
			attackRange.extentX = 50;
			attackRange.extentY = 10;
			attackRange.yaw = IngameDataList[t.id].rz_;

			for (int i = 1; i < 5; ++i) {
				if (IngameMapDataList[t.id / 5].player_ids_[i] == -1)
					continue;
				Circle player;
				player.center.x = IngameDataList[t.id + i].x_;
				player.center.y = IngameDataList[t.id + i].y_;
				player.r = IngameDataList[t.id + i].r_;
				if ((IngameDataList[t.id + i].z_ - IngameDataList[t.id+i].extent_z_)> (IngameDataList[t.id].z_ + IngameDataList[t.id].extent_z_))
					continue;

				if (IngameDataList[t.id + i].z_ + IngameDataList[t.id + i].extent_z_ < IngameDataList[t.id].z_ - IngameDataList[t.id].extent_z_)
					continue;

				if (!ArePlayerHitted(player, attackRange)) {
					cout << "NoHitted" << endl;
					continue;
				}

				IngameDataList[t.id + i].hp_ -= 200;
				int hittedPlayerId = IngameMapDataList[t.id / 5].player_ids_[i];
				if (IngameDataList[t.id + i].hp_ > 0) {
					for (int id : IngameMapDataList[t.id / 5].player_ids_) {
						if (id == -1) continue;
						clients[id]->SendOtherPlayerHittedPacket(hittedPlayerId, IngameDataList[t.id + i].hp_);
					}
				}
				else {
					for (int id : IngameMapDataList[t.id / 5].player_ids_) {
						if (id == -1) continue;
						clients[id]->SendOtherPlayerDeadPacket(hittedPlayerId);
					}
				}
			}
			TimerQueue.pop();
			continue;
		}
		}
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
		BombTimerQueue.pop();

		t.time_interval += 0.01f;
		Vector3D newPosition;
		newPosition = parabolicMotion(t.bomb.pos_, t.bomb.initialVelocity_, acceleration, t.time_interval);
		t.bomb.pos_ = newPosition;

		if (!BombCollisionTest(t.id, t.room_num, t.bomb.pos_.x, t.bomb.pos_.y, t.bomb.pos_.z, t.bomb.r_, t.bomb.index_, t.bomb.bomb_type_)) {
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
				data.r_ = 23.845644;
				data.extent_z_ = 72.056931;
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
				data2.r_ = 27.04608;
				data2.extent_z_ = 49.669067;
				data2.hp_ = 2000;
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
		int roomNum = clients[c_id]->ingame_num_ / 5;
		IngameMapData& igmd = IngameMapDataList[roomNum];

		CS_MAP_LOADED_PACKET* p = reinterpret_cast<CS_MAP_LOADED_PACKET*>(packet);
		
		igmd.in_game_users_num_++;

		if (igmd.in_game_users_num_!=2) break;	// [need to edit]

		
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
		if (5 >= IngameDataList[ingame_num_].role_)
			break;
		CS_ATTACK_PACKET* p = reinterpret_cast<CS_ATTACK_PACKET*>(packet);

		for (int id : IngameMapDataList[room_num_].player_ids_) {
			if (id == -1) continue;
			clients[id]->SendAttackPacket(c_id);
		}
		Timer timer;
		timer.id = ingame_num_;
		timer.status = ChaserHit;
		timer.prev_time = std::chrono::high_resolution_clock::now();
		TimerQueue.push(timer);
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
		if (p->item == 1)
			timer.status = ItemBoxOpen;
		else if (p->item == 2)
			timer.status = FuseBoxOpen;
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

		if (IngameDataList[ingame_num_].bomb_type_ == NoBomb) {
			IngameDataList[ingame_num_].bomb_type_ = p->bombType;
			IngameDataList[ingame_num_].bomb_index_ = p->itemBoxIndex;
			IngameMapDataList[room_num_].ItemBoxes_[p->itemBoxIndex].bomb_.bomb_type_ = NoBomb;
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
		bomb.rx_ = p->rx;
		bomb.ry_ = p->ry;
		bomb.rz_ = p->rz;
		bomb.speed_ = BOMB_SPEED;
		bomb.initialVelocity_ = bomb.calculateInitialVelocity();
		bomb.bomb_type_ = IngameDataList[ingame_num_].bomb_type_;
		bomb.index_ = IngameDataList[ingame_num_].bomb_index_;

		BombTimer timer;
		timer.id = ingame_num_;
		timer.bomb = bomb;
		timer.room_num = room_num_;
		timer.time_interval = 0;
		BombTimerQueue.push(timer);

		IngameDataList[ingame_num_].bomb_type_ = NoBomb;
		for (int id : IngameMapDataList[room_num_].player_ids_) {
			if (id == -1) continue;
			clients[id]->SendCannonFirePacket(c_id ,bomb);
		}
		break;
	}
	
	case CS_USE_SKILL: {
		CS_USE_SKILL_PACKET* p = reinterpret_cast<CS_USE_SKILL_PACKET*>(packet);
		std::chrono::high_resolution_clock::time_point	now;
		now = std::chrono::high_resolution_clock::now();
		auto time_diff = std::chrono::duration_cast<std::chrono::seconds>(now - IngameDataList[ingame_num_].last_skill_time);

		
		if (time_diff.count() < IngameDataList[ingame_num_].skill_cool_down_)
			break;
		IngameDataList[ingame_num_].last_skill_time = now;
		for (int id : IngameMapDataList[room_num_].player_ids_) {
			if (id == -1) continue;
			clients[id]->SendUseSkillPacket(c_id);
		}
		break;
	}

	case CS_ESCAPE: {
		if (ingame_num_ % 5 == 0) break;
		int index = -1;
		int remain_clients = 0;
		int remain_id = 0;
		int remain_index = 0;
		for (int id : IngameMapDataList[room_num_].player_ids_) {
			index++;
			if (id == -1) continue;
			if (id == c_id) {
				IngameMapDataList[room_num_].player_ids_[index] = -1;
				continue;
			}
			remain_clients++;
			remain_id = id;
			remain_index = index;
			clients[id]->SendRemovePlayerPacket(c_id);
		}
		clients[c_id]->SendEscapePacket(c_id, true, 0);
		if (remain_clients == 1) {
			clients[remain_id]->SendEscapePacket(remain_id, false, 0);
			IngameMapDataList[room_num_].player_ids_[remain_index] = -1;
		}
		IngameMapDataList[room_num_].finished_player_list_.emplace_back(c_id);
		/*for (int id : IngameMapDataList[room_num_].finished_player_list_) {
			clients[id]->SendEscapePacket(c_id, IngameDataList[ingame_num_].die_, IngameDataList[ingame_num_].score_);
			if (id == c_id)
				continue;
			clients[c_id]->SendEscapePacket(id, IngameDataList[clients[id]->ingame_num_].die_, IngameDataList[clients[id]->ingame_num_].score_);
	
		}*/
		bool game_finished = true;;
		for (int id : IngameMapDataList[room_num_].player_ids_) {
			if (id != -1)
				game_finished = false;
		}
		if (!game_finished)
			break;
		for (int i = 0; i < 5; ++i) {
			IngameDataList.unsafe_erase(room_num_*5 + i);
		}
		IngameMapDataList.unsafe_erase(room_num_);
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
		unsigned char* buf = data_;
		while (0 < dataToProcess) {
			if (0 == curr_packet_size_) {
				curr_packet_size_ = buf[0];
				if (buf[0] > 255) {
					cout << "Invalid Packet Size [ << buf[0] << ]\n";
					exit(-1);
				}
			}
			int needToBuild = curr_packet_size_ - prev_data_size_;
			if (dataToProcess + prev_data_size_ >= curr_packet_size_) {
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
void cSession::SendItemBoxOpenedPacket(int index, BombType bomb_type)
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

void cSession::SendRemoveJellyPacket(int index, int bomb_index)
{
	SC_REMOVE_JELLY_PACKET p;
	p.size = sizeof(SC_REMOVE_JELLY_PACKET);
	p.type = SC_REMOVE_JELLY;
	p.jellyIndex = index;
	p.bomb_index = bomb_index;
	SendPacket(&p);
}

void cSession::SendUseSkillPacket(int c_id)
{
	SC_USE_SKILL_PACKET p;
	p.size = sizeof(SC_USE_SKILL_PACKET);
	p.type = SC_USE_SKILL;
	p.id = c_id;
	SendPacket(&p);
}

void cSession::SendChaserResurrectionPacket(int c_id)
{
	SC_CHASER_RESURRECTION_PACKET p;
	p.size = sizeof(SC_CHASER_RESURRECTION_PACKET);
	p.type = SC_CHASER_RESURRECTION;
	p.id = c_id;
	p.x = IngameDataList[c_id].x_;
	p.y = IngameDataList[c_id].y_;
	p.z = IngameDataList[c_id].z_;
	p.rx = IngameDataList[c_id].rx_;
	p.ry = IngameDataList[c_id].ry_;
	p.rz = IngameDataList[c_id].rz_;
	p.hp = IngameDataList[c_id].hp_;
	SendPacket(&p);
}

void cSession::SendEscapePacket(int c_id, bool win, int score)
{
	SC_ESCAPE_PACKET p;
	p.size = sizeof(SC_ESCAPE_PACKET);
	p.type = SC_ESCAPE;
	p.id = c_id;
	p.win = win;
	p.score = score;
	SendPacket(&p);
}

void cSession::SendRemovePlayerPacket(int c_id)
{
	SC_REMOVE_PLAYER_PACKET p;
	p.size = sizeof(SC_REMOVE_PLAYER_PACKET);
	p.type = SC_REMOVE_PLAYER;
	p.id = c_id;
	SendPacket(&p);
}
