#include "Session.h"

using namespace std;

extern unordered_map<int, unordered_map<int, vector<Object>>> OBJS;		
extern unordered_map<int, array<Jelly, MAX_JELLY_NUM>> Jellys;	// 젤리 위치 정보
thread_local unordered_map<int, shared_ptr<cSession>> clients;

thread_local unordered_map<std::string, array<std::string, 2>> UserInfo;
thread_local unordered_set<std::string> UserName;
extern thread_local int NowUserNum;
extern thread_local int TotalPlayer;
extern unordered_map<int, array <FuseBox, MAX_FUSE_BOX_NUM>> FuseBoxes;	// 퓨즈 박스 위치 정보					

extern thread_local int NowRoomNumber;

thread_local unordered_map<int, IngameMapData> IngameMapDataList;  
thread_local unordered_map<int, cIngameData>	IngameDataList;
thread_local unordered_map<int, array<int, MAX_ROOM_PLAYER>> WaitingMap;

extern vector<boost::asio::steady_timer> timers;

thread_local unordered_map<int, int> WaitingQueue;

int MapJelliesNum[TOTAL_NUMBER_OF_MAPS+1] = { 0,55,25 ,51,51};

int now_map_number_ = 0;



enum TimerName { ItemBoxOpen, FuseBoxOpen, ChaserResurrection, ChaserHit, INVINCIBLE };
struct Timer {
	int			id;
	int			c_id;
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
	int		cl_id;
	int		room_num;
	Bomb	bomb;
	float	time_interval;
};

thread_local queue<Timer> TimerQueue;
thread_local queue<BombTimer> BombTimerQueue;


struct Vector2D {
	float x;
	float y;
	Vector2D operator-(const Vector2D& rhs) const {
		return { x - rhs.x, y - rhs.y };
	}

	// 벡터의 길이(크기) 계산
	float magnitude() const {
		return std::sqrt(x * x + y * y);
	}

	// 회전 변환
	Vector2D rotate(float yaw) const {
		float rad = yaw * static_cast<float>(M_PI / 180.0); // 각도를 라디안으로 변환
		float cos_rad = std::cos(rad);
		float sin_rad = std::sin(rad);
		if (std::abs(cos_rad) < std::numeric_limits<float>::epsilon())
			cos_rad = 0;
		if (std::abs(sin_rad) < std::numeric_limits<float>::epsilon())
			sin_rad = 0;
		return {
			y* sin_rad + x * cos_rad,
			y * cos_rad - x *sin_rad
		};
	}
};
struct Sphere {
	Vector2D center;
	float z;
	float r;
};

struct Circle {
	Vector2D center;
	float r;
};

typedef struct Rectangle {
	Vector2D center;
	float extentX;
	float extentY;
	float yaw;
}rectangle;


Vector3D parabolicMotion(const Vector3D& initialPosition, const Vector3D& initialVelocity, const Vector3D& acceleration, float time) {
	Vector3D halfAccel = acceleration * 0.5;
	Vector3D displacement = initialVelocity * time + halfAccel * (time * time);
	return initialPosition + displacement;
}
bool AreCircleAndSquareColliding(const Circle& circle, const rectangle& rect)
{
	float dx = circle.center.x - rect.center.x;
	float dy = circle.center.y - rect.center.y;
	float dist = sqrt(dx * dx + dy * dy);

	float max_sq = sqrt(rect.extentX * rect.extentX + rect.extentY * rect.extentY);

	if (dist > circle.r + max_sq)
		return false;
	return true;
}
bool ArePlayerHitted(Circle& circle, rectangle& rect)
{
	Vector2D newCircle = {circle.center.x-rect.center.x, circle.center.y-rect.center.y};
	newCircle = newCircle.rotate(rect.yaw);
	float closestX = std::max(-rect.extentX+CHASER_HIT_RANGE, std::min(rect.extentX + CHASER_HIT_RANGE, newCircle.x));
	float closestY = std::max(-rect.extentY, std::min(rect.extentY, newCircle.y));
	Vector2D closestPoint = { closestX, closestY };
	float distance = (newCircle - closestPoint).magnitude();
	return distance <= circle.r;
}
bool AreCircleAndRectangleColliding(const Circle& circle, const rectangle& rect)
{
	Vector2D relativeCenter = rect.center - circle.center;
	Vector2D rotatedRelativeCenter = relativeCenter.rotate(-rect.yaw);
	float closestX = std::max(-rect.extentX, std::min(rect.extentX, rotatedRelativeCenter.x));
	float closestY = std::max(-rect.extentY, std::min(rect.extentY, rotatedRelativeCenter.y));
	Vector2D closestPoint = { closestX, closestY };

	float distance = (rotatedRelativeCenter - closestPoint).magnitude();

	return distance <= circle.r;
}

bool AreCircleAndCircleColliding(const Sphere& bomb, const Sphere& player, float player_extent_z) {
	if (bomb.z > player.z+ player_extent_z) 
		return false;
	if (bomb.z < player.z - player_extent_z)
		return false;

	float distance = float(sqrt(pow(player.center.x - bomb.center.x, 2) + pow(player.center.y - bomb.center.y, 2)));
	if (distance <= (bomb.r + player.r)) {
		return true;
	}
	return false;
}
void RenewColArea(const int c_id, const Circle& circle)
{
	IngameDataList[c_id].col_area_.clear();
	rectangle rec1;

	int minRow = max(0, (static_cast<int>(circle.center.x)  / COL_SECTOR_SIZE) - 1);
	int maxRow = min(static_cast<int>(circle.center.x)  / COL_SECTOR_SIZE + 1, static_cast<int>(ceil(MAP_X / COL_SECTOR_SIZE)));
	int minCol = max(0, (static_cast<int>(circle.center.y) / COL_SECTOR_SIZE) - 1);
	int maxCol = min(static_cast<int>(circle.center.y) / COL_SECTOR_SIZE + 1, static_cast<int>(ceil(MAP_Y / COL_SECTOR_SIZE)));

	for (int row = minRow; row <= maxRow; ++row) {
		for (int col = minCol; col <= maxCol; ++col) {
			rec1 = { {float(row) * 800 + 400, float(col) * 800 + 400}, 400, 400, 0 };
			if (AreCircleAndSquareColliding(circle, rec1)) {
				IngameDataList[c_id].col_area_.push_back(row + col * 32);
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
		float localX = (sphere.center.x - obj.pos_x_) * static_cast<float>(cos(-obj.yaw_ * M_PI / 180.0)) -
			(sphere.center.y - obj.pos_y_) * static_cast<float>(sin(-obj.yaw_ * M_PI / 180.0));
		float localY = (sphere.center.x - obj.pos_x_) * static_cast<float>(sin(-obj.yaw_ * M_PI / 180.0)) +
			(sphere.center.y - obj.pos_y_) * static_cast<float>(cos(-obj.yaw_ * M_PI / 180.0));


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
		float localX = (sphere.center.x - jelly.pos_x_) * static_cast<float>(cos(-jelly.yaw_ * M_PI / 180.0f)) -
			(sphere.center.y - jelly.pos_y_) * static_cast<float>(sin(-jelly.yaw_ * M_PI / 180.0f));
		float localY = (sphere.center.x - jelly.pos_x_) * static_cast<float>(sin(-jelly.yaw_ * M_PI / 180.0f)) +
			(sphere.center.y - jelly.pos_y_) * static_cast<float>(cos(-jelly.yaw_ * M_PI / 180.0f));


		bool collisionX = std::abs(localX) <= jelly.extent_x_ + sphere.r;
		bool collisionY = std::abs(localY) <= jelly.extent_y_ + sphere.r;

		return collisionX && collisionY;
	}
	return false;
}

bool CollisionTest(int c_id, float x, float y, float z, float r) {
	Sphere sphere;
	sphere.center = { x, y };
	sphere.z = z;
	sphere.r = r;
	Circle circle;
	circle.center = { x,y };
	circle.r = r;
	RenewColArea(c_id, circle);

	for (auto& colArea : IngameDataList[c_id].col_area_) {
		for (auto& colObject : OBJS[IngameDataList[c_id].map_num_][colArea]) {
			if (ArePlayerColliding(sphere, colObject)) {
				return true;
			}
		}
	}
	return false;
}

bool BombCollisionTest(const int c_id, const int cl_id, const int room_num, const float x, const float y, const float z, const float r, const int bomb_index, const BombType bomb_type) {
	Sphere sphere;
	sphere.center = { x,y };
	sphere.z = z;
	sphere.r = r;
	
	Circle circle;
	circle.center = { x,y };
	circle.r = r;

	Sphere player;
	player.center.x = IngameDataList[room_num*5].x_;
	player.center.y = IngameDataList[room_num * 5].y_;
	player.z = IngameDataList[room_num * 5].z_;
	player.r = IngameDataList[room_num * 5].r_;

	int chaserId = IngameMapDataList[room_num].player_ids_[0];
	IngameMapData& igmd = IngameMapDataList[room_num];

	vector<int> colAreas;

	int minRow = max(0, (static_cast<int>(sphere.center.x) / COL_SECTOR_SIZE) - 1);
	int maxRow = min(static_cast<int>(sphere.center.x) / COL_SECTOR_SIZE + 1, static_cast<int>(ceil(MAP_X / COL_SECTOR_SIZE)));
	int minCol = max(0, (static_cast<int>(sphere.center.y)/ COL_SECTOR_SIZE) - 1);
	int maxCol = min(static_cast<int>(sphere.center.y) / COL_SECTOR_SIZE + 1, static_cast<int>(ceil(MAP_Y / COL_SECTOR_SIZE)));



	for (int row = minRow; row <= maxRow; ++row) {
		for (int col = minCol; col <= maxCol; ++col) {
			rectangle rec1 = { { float(row) * 800 + 400, float(col) * 800 + 400}, 400, 400, 0 };
			if (AreCircleAndSquareColliding(circle, rec1)) {
				colAreas.push_back(row + col * 32);
			}
		}
	}

	for (auto& colArea : colAreas) {
		for (auto& colObject : OBJS[IngameMapDataList[room_num].map_num_][colArea]) {
			//cout << "물체의 z : " << colObject.pos_z_<<"\n";
			//cout << "폭탄의 z : " << sphere.z << "\n";
			if (ArePlayerColliding(sphere, colObject)) {
				cout << "물체가 폭탄을 맞음\n";
				return true;
			}
		}
	}

	int jelly_index = -1;
	for (auto& jelly : Jellys[clients[cl_id]->map_num_]) {
		jelly_index++;
		if (igmd.jellies[jelly_index] == 0)
			continue;
		if (AreBombAndJellyColliding(sphere, jelly)) {
			cout << "젤리가 폭탄을 맞음\n";
			for (int id : IngameMapDataList[room_num].player_ids_) {
				if (id == -1) continue;
				clients[id]->SendRemoveJellyPacket(jelly.index_, x,y,z);
			}
			igmd.jellies[jelly_index] = 0;
			return true;
		}
	}
	sphere.r += 10;
	if (AreCircleAndCircleColliding(sphere, player, IngameDataList[room_num*5].extent_z_)){
		cout << "술래가 폭탄을 맞음\n";
		if (bomb_type == Explosion) {
			if(IngameDataList[c_id].damage_up_){
				IngameDataList[c_id].damage_up_ = false;
				IngameDataList[room_num * 5].hp_ -= 400;
			}
			else
				IngameDataList[room_num * 5].hp_ -= 200;

			if (IngameDataList[room_num*5].hp_ <= 0) {
				for (int id : IngameMapDataList[room_num].player_ids_) {
					if (id == -1) continue;
					cout << "Send Dead to Client : " << id << "\n";
					clients[id]->SendOtherPlayerDeadPacket(chaserId);
				}
				Timer deadTimer;
				deadTimer.id = room_num*5;
				deadTimer.c_id = chaserId;
				deadTimer.status = ChaserResurrection;
				deadTimer.current_time = std::chrono::high_resolution_clock::now();
				TimerQueue.push(deadTimer);
			}
			else {
				for (int id : IngameMapDataList[room_num].player_ids_) {
					if (id == -1) continue;
					clients[id]->SendOtherPlayerHittedPacket(chaserId, IngameDataList[room_num*5].hp_, Explosion);
				}
			}
		}
		else if (bomb_type == Stun) {
			if (IngameDataList[c_id].damage_up_) {
				IngameDataList[c_id].damage_up_ = false;
				IngameDataList[room_num * 5].hp_ -= 200;
			}
			else
				IngameDataList[room_num * 5].hp_ -= 100;
			if (IngameDataList[room_num * 5].hp_ <= 0) {
				for (int id : IngameMapDataList[room_num].player_ids_) {
					if (id == -1) continue;
					cout << "Send Dead to Client : " << id << "\n";

					clients[id]->SendOtherPlayerDeadPacket(chaserId);
				}
				Timer deadTimer;
				deadTimer.id = room_num * 5;
				deadTimer.c_id = chaserId;
				deadTimer.status = ChaserResurrection;
				deadTimer.current_time = std::chrono::high_resolution_clock::now();
				TimerQueue.push(deadTimer);
			}
			else {
				for (int id : IngameMapDataList[room_num].player_ids_) {
					if (id == -1) continue;
					clients[id]->SendOtherPlayerHittedPacket(chaserId, IngameDataList[room_num * 5].hp_, Stun);
				}
			}
		}
		else if (bomb_type == Blind) {
			if (IngameDataList[c_id].damage_up_) {
				IngameDataList[room_num * 5].hp_ -= 100;
				IngameDataList[c_id].damage_up_ = false;
			}
			else
				IngameDataList[room_num * 5].hp_ -= 50;

			if (IngameDataList[room_num * 5].hp_ <= 0) {
				for (int id : IngameMapDataList[room_num].player_ids_) {
					if (id == -1) continue;
					cout << "Send Dead to Client : " << id << "\n";
					clients[id]->SendOtherPlayerDeadPacket(chaserId);
				}
				Timer deadTimer;
				deadTimer.id = room_num * 5;
				deadTimer.c_id = chaserId;
				deadTimer.status = ChaserResurrection;
				deadTimer.current_time = std::chrono::high_resolution_clock::now();
				TimerQueue.push(deadTimer);
			}
			else {
				for (int id : IngameMapDataList[room_num].player_ids_) {
					if (id == -1) continue;
					clients[id]->SendOtherPlayerHittedPacket(chaserId, IngameDataList[room_num * 5].hp_, Blind);
				}
			}
		}
		return true;
	}

	return false;
}

Vector3D yawToDirectionVector(float yawDegrees) {
	float yawRadians = yawDegrees * static_cast<float>(M_PI / 180.0f);
	float x = cos(yawRadians);
	float y = sin(yawRadians);
	return Vector3D(x, y, 0);
}

float angleBetween(const Vector3D& v1, const Vector3D& v2) {
	float dotProduct = v1.dot(v2);
	float magnitudeProduct = v1.magnitude() * v2.magnitude();
	return acos(dotProduct / magnitudeProduct) * static_cast<float>(180.0 / M_PI);  // Radians to degrees
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

		int room_num = clients[t.c_id]->room_num_;

		t.prev_time = t.current_time;

		if ((t.status != ChaserHit) && (t.status != INVINCIBLE)) {
			t.current_time = std::chrono::high_resolution_clock::now();
		}

		switch (t.status) {
		case ItemBoxOpen: {
			if (IngameMapDataList[room_num].ItemBoxes_[t.index].interaction_id_ == -1) {
				TimerQueue.pop();
				continue;
			}
			auto interaction_time = std::chrono::duration_cast<std::chrono::microseconds>(t.current_time - t.prev_time);
			IngameMapDataList[room_num].ItemBoxes_[t.index].progress_ += interaction_time.count() / static_cast<float>(3.0 * SEC_TO_MICRO);
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
			IngameMapDataList[room_num].fuse_boxes_[serverFuseBoxIndex].progress_ += interaction_time.count() / static_cast<float>(5.0 * SEC_TO_MICRO);
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
				IngameDataList[t.id].resurrectionCooldown_ = float(10) * IngameDataList[t.id].resurrectionCount;
				IngameDataList[t.id].hp_ = IngameDataList[t.id].before_hp_ + 400;
				IngameDataList[t.id].before_hp_ += 400;
				for (int id : IngameMapDataList[room_num].player_ids_) {
					if (id == -1) continue;
					cout << "Send Resurrection to Client : " << id << "\n";
					cout << "HP : " << IngameDataList[t.id].before_hp_ << "\n";
					clients[id]->SendChaserResurrectionPacket(IngameMapDataList[room_num].player_ids_[0]);
				}
				TimerQueue.pop();
				continue;
			}
			break;
		}
		case ChaserHit: {
			auto timeDiff = std::chrono::high_resolution_clock::now() - t.prev_time;
			if (std::chrono::duration_cast<std::chrono::milliseconds>(timeDiff).count() < 500) {
				break;
			}

			rectangle attackRange;
			attackRange.center.x = IngameDataList[t.id].x_;
			attackRange.center.y = IngameDataList[t.id].y_;
			attackRange.extentX = 70;
			attackRange.extentY = 20;
			attackRange.yaw = IngameDataList[t.id].rz_;

			for (int i = 1; i < 5; ++i) {
				if (IngameMapDataList[room_num].player_ids_[i] == -1)
					continue;
				if (IngameDataList[t.id + i].Invincible == true)
					continue;
				if (IngameDataList[t.id + i].die_)
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
				int hittedPlayerId = IngameMapDataList[room_num].player_ids_[i];
				if (IngameDataList[t.id + i].hp_ > 0) {
					for (int id : IngameMapDataList[room_num].player_ids_) {
						if (id == -1) continue;
						clients[id]->SendOtherPlayerHittedPacket(hittedPlayerId, IngameDataList[t.id + i].hp_);
					}
				}
				else {
					IngameMapDataList[room_num].dead_player_count++;
					IngameDataList[t.id + i].die_ = true;
					if (IngameMapDataList[room_num].dead_player_count == MAX_ROOM_PLAYER - 1) {
						for (int id : IngameMapDataList[room_num].player_ids_) {
							if (id == -1)continue;
							if (IngameMapDataList[room_num].escape_success) {
								clients[id]->SendEscapePacket(id, true, 0);
							}
							else {
								clients[id]->SendChaserWinPacket();
							}
							IngameDataList.erase(t.id);
							clients.erase(id);
						}
						IngameMapDataList.erase(room_num);
					}
					else {
						for (int id : IngameMapDataList[room_num].player_ids_) {
							if (id == -1) continue;
							clients[id]->SendOtherPlayerDeadPacket(hittedPlayerId);
						}
					}
				}
			}
			TimerQueue.pop();
			continue;
		}
		case INVINCIBLE:{
			auto timeDiff = std::chrono::high_resolution_clock::now() - t.prev_time;
			if (std::chrono::duration_cast<std::chrono::milliseconds>(timeDiff).count() < 5000) {
				break;
			}
			IngameDataList[t.id].Invincible = false;
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
	Vector3D acceleration = { 0.f, 0.f, -9.8f };
	for (int i = 0; i < BombTimerQueue.size(); ++i) {
		BombTimer& t = BombTimerQueue.front();
		BombTimerQueue.pop();
		if (clients[t.cl_id] == nullptr)
			continue;
		t.time_interval += 0.01f;
		Vector3D newPosition;
		newPosition = parabolicMotion(t.bomb.pos_, t.bomb.initialVelocity_, acceleration, t.time_interval);
		t.bomb.pos_ = newPosition;

		if (t.bomb.pos_.z < 0) {
			cout << "폭탄이 땅에 맞음\n";
			continue;
		}
		if (!BombCollisionTest(t.id,t.cl_id,  t.room_num, t.bomb.pos_.x, t.bomb.pos_.y, t.bomb.pos_.z, t.bomb.r_, t.bomb.index_, t.bomb.bomb_type_)) {
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
	
	case CS_CONNECT_GAME_SERVER: {
		cout << "CS_CONNECT_GAME_SERVER_PACKET to client " << c_id << "\n";
		
		CS_CONNECT_GAME_SERVER_PACKET* p = reinterpret_cast<CS_CONNECT_GAME_SERVER_PACKET*>(packet);
		strcpy(role_, p->role);
		charactor_num_ = p->charactorNum;
		bool isGroupReady = false;
		
		int my_count = WaitingQueue[p->GroupNum]++;
		if (my_count == MAX_ROOM_PLAYER-1)
			isGroupReady = true;
		WaitingMap[p->GroupNum][my_count] = c_id;

		if (isGroupReady) {
			WaitingQueue.erase(p->GroupNum);
			IngameMapData igmd;
			int player_count = 1;
			for (int id : WaitingMap[p->GroupNum]) {
				if (clients[id]->charactor_num_ >= 6) {
					igmd.player_ids_[0] = id;
					cout << id << "얘가 술래\n";
				}
				else {
					igmd.player_ids_[player_count++] = id;
					cout << id << "얘는 도망자\n";
				}
			}
			// 최종 발표용
			int mapId = (now_map_number_++) % 4 + 1; //rand() % 2 + 1;
			// 원래 게임용
			//int mapId = rand() % TOTAL_NUMBER_OF_MAPS + 1;
			int patternId = rand() % 3 + 1;
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


				igmd.fuse_boxes_[i].extent_x_ = FuseBoxes[mapId][index].extent_x_;
				igmd.fuse_boxes_[i].extent_y_ = FuseBoxes[mapId][index].extent_y_;
				igmd.fuse_boxes_[i].extent_z_ = FuseBoxes[mapId][index].extent_z_;
				igmd.fuse_boxes_[i].pos_x_ = FuseBoxes[mapId][index].pos_x_;
				igmd.fuse_boxes_[i].pos_y_ = FuseBoxes[mapId][index].pos_y_;
				igmd.fuse_boxes_[i].pos_z_ = FuseBoxes[mapId][index].pos_z_;
				igmd.fuse_boxes_[i].yaw_ = FuseBoxes[mapId][index].yaw_;
				igmd.fuse_boxes_[i].roll_ = FuseBoxes[mapId][index].roll_;
				igmd.fuse_boxes_[i].pitch_ = FuseBoxes[mapId][index].pitch_;

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

			for (int i = 0; i < MapJelliesNum[mapId]; ++i) {
				igmd.jellies[i] = 1;
			}

			igmd.map_num_ = mapId;
			SC_MAP_INFO_PACKET mapinfo_packet;
			mapinfo_packet.size = sizeof(mapinfo_packet);
			mapinfo_packet.type = SC_MAP_INFO;
			mapinfo_packet.mapid = mapId;
			mapinfo_packet.patternid = patternId;
			for (int i = 0; i < 8; ++i) {
				mapinfo_packet.fusebox[i] = igmd.fuse_box_list_[i];
				mapinfo_packet.fusebox_color[i] = fuseBoxColorList[i];
			}

			int roomNum = NowRoomNumber++;

			IngameMapDataList[roomNum] = igmd;

			for (int id : igmd.player_ids_) {
				if (id == -1)
					continue;
				clients[id]->SendMapInfoPacket(mapinfo_packet);
				clients[id]->room_num_ = roomNum;
				clients[id]->map_num_ = mapId;
			}

			for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
				cIngameData data;
				data.room_num_ = roomNum;

				if (i == 0) {
					data.r_ = 23.845644f;
					data.extent_z_ = 72.056931f;
					data.hp_ = 600;

				}
				else {
					data.r_ = 27.04608f;
					data.extent_z_ = 49.669067f;
					data.hp_ = 2000;
				}

				data.role_ = clients[igmd.player_ids_[i]]->charactor_num_;
				data.user_name_ = clients[igmd.player_ids_[i]]->user_name_;
				data.my_client_num_ = igmd.player_ids_[i];
				data.my_ingame_num_ = roomNum * 5+i;
				data.map_num_ = mapId;
				clients[igmd.player_ids_[i]]->ingame_num_ = data.my_ingame_num_;
				IngameDataList[data.my_ingame_num_] = data;
			}
		}
		break;
	}

	case CS_MAP_LOADED: {
		IngameMapData& igmd = IngameMapDataList[room_num_];

		CS_MAP_LOADED_PACKET* p = reinterpret_cast<CS_MAP_LOADED_PACKET*>(packet);
		
		igmd.in_game_users_num_++;

		if (igmd.in_game_users_num_ < MAX_ROOM_PLAYER)
			break;

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
		if (map_num_ != 4) {
			if (!CollisionTest(ingame_num_, p->x, p->y, p->z, IngameDataList[ingame_num_].r_)) {
				IngameDataList[ingame_num_].x_ = p->x;
				IngameDataList[ingame_num_].y_ = p->y;
				IngameDataList[ingame_num_].z_ = p->z;
			}
			else {
				static int num = 0;
				cout << c_id << " player in Wrong Place !" << num++ << endl;
			}
		}
		else {
			IngameDataList[ingame_num_].x_ = p->x;
			IngameDataList[ingame_num_].y_ = p->y;
			IngameDataList[ingame_num_].z_ = p->z;
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
		if (IngameDataList[ingame_num_].role_ <= 5)
			break;
		CS_ATTACK_PACKET* p = reinterpret_cast<CS_ATTACK_PACKET*>(packet);

		for (int id : IngameMapDataList[room_num_].player_ids_) {
			if (id == -1) continue;
			clients[id]->SendAttackPacket(c_id);
		}

		Timer timer;
		timer.id = ingame_num_;
		timer.c_id = c_id;
		timer.status = ChaserHit;
		timer.current_time = std::chrono::high_resolution_clock::now();
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
		timer.c_id = c_id;
		if (p->item == 1)
			timer.status = ItemBoxOpen;
		else if (p->item == 2)
			timer.status = FuseBoxOpen;
		timer.index = p->index;
		timer.current_time = std::chrono::high_resolution_clock::now();
		TimerQueue.push(timer);
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
		timer.cl_id = c_id;
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

		cout << "recv use skill packet to client [ " << c_id << " ]\n";
		if (time_diff.count() < IngameDataList[ingame_num_].skill_cool_down_)
			break;

		IngameDataList[ingame_num_].last_skill_time = now;
		SkillType st;
		switch (p->skill_type)
		{
		case SkillType::CowBoy: {
			st = CowBoy;
			break;
		}
		case SkillType::Dancer: {
			st = Dancer;
			Timer timer;
			timer.id = ingame_num_;
			timer.c_id = c_id;
			timer.status = INVINCIBLE;
			timer.current_time = std::chrono::high_resolution_clock::now();
			TimerQueue.push(timer);
			IngameDataList[ingame_num_].Invincible = true;
			break;
		}
		case SkillType::Soldier: {
			st = Soldier;
			break;
		}
		case SkillType::Student: {
			st = Student;
			while (st == Student) {
				st = SkillType(rand() % 5) ;
			}
			cout <<c_id<< "에게학생 패킷 보냄 \n";
			for (int id : IngameMapDataList[room_num_].player_ids_) {
				if (id == -1) continue;
				clients[id]->SendSkillChoosedPacket(st);
			}
			return;
		}
		case SkillType::Warrior: {
			st = Warrior;
			IngameDataList[ingame_num_].damage_up_ = true;
			break;
		}
		case SkillType::Chaser1: {
			st = Chaser1;
			break;
		}
		case SkillType::Chaser2: {
			st = Chaser2;
			break;
		}
		default:
			break;
		}
		for (int id : IngameMapDataList[room_num_].player_ids_) {
			if (id == -1) continue;
			clients[id]->SendUseSkillPacket(c_id ,st);
		}
		break;
	}

	case CS_ESCAPE: {
		if (ingame_num_ % 5 == 0) break;
		int index = -1;
		int remain_clients = 0;
		int remain_id = 0;
		int remain_index = 0;
		if (IngameMapDataList[room_num_].dead_player_count < MAX_ROOM_PLAYER - 2) {
			for (int id : IngameMapDataList[room_num_].player_ids_) {
				index++;
				if (id == -1) continue;
				if (id == c_id) {
					IngameMapDataList[room_num_].player_ids_[index] = -1;
					continue;
				}
				clients[id]->SendRemovePlayerPacket(c_id);
			}
			clients[c_id]->SendEscapePacket(c_id, true, 0);
			IngameMapDataList[room_num_].escape_success = true;
			IngameMapDataList[room_num_].dead_player_count++;
			escape_ = true;
		}
		else {
			int now_indexx = -1;
			for (int id : IngameMapDataList[room_num_].player_ids_) {
				now_indexx++;
				if (id == -1) continue;
				clients[id]->SendEscapePacket(id, true, 0);
				IngameMapDataList[room_num_].player_ids_[now_indexx] = -1;
			}
		}

		bool game_finished = true;
		for (int id : IngameMapDataList[room_num_].player_ids_) {
			if (id != -1)
				game_finished = false;
		}
		if (!game_finished)
			break;
		for (int i = 0; i < 5; ++i) {
			IngameDataList.erase(room_num_*5 + i);
		}
		IngameMapDataList.erase(room_num_);
		break;
	}

	case CS_GOTO_LOBBY: {
		socket_.close();
		break;
	}

	case CS_PICK_UP_INK: {
		if (charactor_num_ > 5)
			break;
		CS_PICK_UP_INK_PACKET* p = reinterpret_cast<CS_PICK_UP_INK_PACKET*>(packet);
		IngameDataList[ingame_num_].bomb_type_ = Blind;
		for (int id : IngameMapDataList[room_num_].player_ids_) {
			if (id == -1) continue;
			clients[id]->SendPickUpInkPacket(c_id);
		}
		break;
	}

	case CS_PICK_UP_STUN: {
		if (charactor_num_ > 5)
			break;
		CS_PICK_UP_STUN_PACKET* p = reinterpret_cast<CS_PICK_UP_STUN_PACKET*>(packet);
		IngameDataList[ingame_num_].bomb_type_ = Stun;
		for (int id : IngameMapDataList[room_num_].player_ids_) {
			if (id == -1) continue;
			clients[id]->SendPickUpStunPacket(c_id);
		}
		break;
	}

	case CS_PICK_UP_EXPLOSION: {
		if (charactor_num_ > 5)
			break;
		CS_PICK_UP_EXPLOSION_PACKET* p = reinterpret_cast<CS_PICK_UP_EXPLOSION_PACKET*>(packet);
		IngameDataList[ingame_num_].bomb_type_ = Explosion;
		for (int id : IngameMapDataList[room_num_].player_ids_) {
			if (id == -1) continue;
			clients[id]->SendPickUpExplosionPacket(c_id);
		}
		break;
	}

	case CS_PORTAL_GAUGE_HALF: {
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
	default: cout << "Invalid Packet From Client [" << c_id << "]  PacketID : " << int(packet[1]) << "\n";
	}
}
void cSession::DoRead()
{
	auto self(shared_from_this());
	socket_.async_read_some(boost::asio::buffer(data_), 
		[this, self](boost::system::error_code ec, std::size_t length) {
		if (ec)
		{
 			if (ec.value() == boost::asio::error::operation_aborted) return;

			if (ec == boost::asio::error::eof ||
				ec == boost::asio::error::connection_reset)
			{
				// 클라이언트가 접속을 끊은 경우 처리
				std::cout << "Client " << my_id_<<" disconnected.\n";

				// 만약 술래가 접속을 종료하면 도망자 승리로 게임을 종료
				if (ingame_num_ % 5 == 0) {
					for (int id : IngameMapDataList[room_num_].player_ids_) {
						if (id == -1) continue;
						if (id != my_id_)
							clients[id]->SendEscapePacket(id, true, IngameDataList[clients[id]->ingame_num_].score_);
						cout << "erase\n";
						IngameDataList.erase(clients[id]->ingame_num_);
						clients.erase(id);
						cout << "erase Complete\n";
						TotalPlayer--;
					}
					// 모든 플레이어들을 내보냈으니 방 삭제
					IngameMapDataList.erase(room_num_);
					// 술래 소켓 닫기
					socket_.close();
				}
				else{	// 술래가 아닌 플레이어가 접속을 종료했을 경우
					IngameMapDataList[room_num_].remain_player_num--;
					if (IngameDataList[ingame_num_].die_ == false) {
						IngameMapDataList[room_num_].dead_player_count++;
						for (int id : IngameMapDataList[room_num_].player_ids_) {
							if (id == -1)
								continue;
							if (id == my_id_)
								continue;
							clients[id]->SendRemovePlayerPacket(my_id_);
						}
					}
					// 만약 술래를 제외한 모든 플레이어가 접속을 종료 했을 경우.
					if (IngameMapDataList[room_num_].dead_player_count == MAX_ROOM_PLAYER-1) {
						for (int id : IngameMapDataList[room_num_].player_ids_) {
							if (id == -1)
								continue;
							if (id == my_id_)
								continue;
							if (IngameMapDataList[room_num_].escape_success) {
								clients[id]->SendEscapePacket(id, true, 0);
							}
							else {
								clients[id]->SendChaserWinPacket();
							}
							IngameDataList.erase(clients[id]->ingame_num_);
							clients.erase(id);

						}
						IngameMapDataList.erase(room_num_);
					}
					else {
						socket_.close();
						for (int& id : IngameMapDataList[room_num_].player_ids_) {
							if (id == my_id_) {
								id = -1;
							}
						}

						IngameDataList.erase(ingame_num_);
						clients.erase(my_id_);
					}
				}
				return;
			}
			else
			{
				// 다른 오류 처리
				cout << "Receive Error on Session[" << my_id_ << "] ERROR_CODE[" << ec << "]\n";
				std::cerr << "Error: " << ec.message() << "\n";
			}
			return; // 오류가 발생했으므로 함수 종료
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
		if (!socket_.is_open()) {
			clients.erase(my_id_);
			return;
		}
		DoRead();
		});
}

void cSession::CloseSocket()
{
	boost::system::error_code ec;
	socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
	if (ec) {
		std::cerr << "Error shutting down socket: " << ec.message() << std::endl;
	}
	socket_.close(ec);
	if (ec) {
		std::cerr << "Error closing socket: " << ec.message() << std::endl;
	}
}

void cSession::DoWrite(unsigned char* packet, std::size_t length, bool closeAfterSend)
{
	auto self(shared_from_this());
	socket_.async_write_some(boost::asio::buffer(packet, length),
		[this, self, packet, length, closeAfterSend](boost::system::error_code ec, std::size_t bytes_transferred)
		{
			if (!ec)
			{
				if (length != bytes_transferred) {
					std::cout << "Incomplete Send occurred on Session[" << my_id_ << "].\n";
				}
				delete packet;

				if (closeAfterSend)
				{
					CloseSocket();
				}
			}
			else
			{
				std::cerr << "Error in DoWrite: " << ec.message() << std::endl;
				delete packet;
				CloseSocket();
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

void cSession::SendPacketAndClose(void* packet)
{
	int packet_size = reinterpret_cast<unsigned char*>(packet)[0];
	unsigned char* buff = new unsigned char[packet_size];
	memcpy(buff, packet, packet_size);
	DoWrite(buff, packet_size,true);
}

void cSession::SendMapInfoPacket(SC_MAP_INFO_PACKET p)
{
	p.id = my_id_;
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
void cSession::SendOtherPlayerHittedPacket(int c_id, int hp, BombType bomb_type)
{
	SC_HITTED_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_HITTED_PACKET);
	p.type = SC_HITTED;
	p.bombType = bomb_type;
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
void cSession::SendItemBoxOpeningPacket(int c_id, int index, float progress)
{
	SC_OPENING_ITEM_BOX_PACKET p;
	p.size = sizeof(SC_OPENING_ITEM_BOX_PACKET);
	p.type = SC_OPENING_ITEM_BOX;
	p.id = c_id;
	p.index = index;
	p.progress = progress;
	SendPacket(&p);
}
void cSession::SendStopOpeningPacket(int c_id, int item, int index, float progress)
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

void cSession::SendFuseBoxOpeningPacket(int c_id, int index, float progress)
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

void cSession::SendRemoveJellyPacket(int index, float x,float y, float z)
{
	SC_REMOVE_JELLY_PACKET p;
	p.size = sizeof(SC_REMOVE_JELLY_PACKET);
	p.type = SC_REMOVE_JELLY;
	p.jellyIndex = index;
	p.b_x = x;
	p.b_y = y;
	p.b_z = z;
	SendPacket(&p);
}

void cSession::SendUseSkillPacket(int c_id, SkillType skill_type)
{
	SC_USE_SKILL_PACKET p;
	p.size = sizeof(SC_USE_SKILL_PACKET);
	p.type = SC_USE_SKILL;
	p.id = c_id;
	p.skill_type = skill_type;
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
	p.runner_win = win;
	p.score = score;
	SendPacketAndClose(&p);
}

void cSession::SendRemovePlayerPacket(int c_id)
{
	SC_REMOVE_PLAYER_PACKET p;
	p.size = sizeof(SC_REMOVE_PLAYER_PACKET);
	p.type = SC_REMOVE_PLAYER;
	p.id = c_id;
	SendPacket(&p);
}

void cSession::SendSkillChoosedPacket(SkillType skill_type)
{
	SC_SKILL_CHOOSED_PACKET p;
	p.size = sizeof(SC_SKILL_CHOOSED_PACKET);
	p.type = SC_SKILL_CHOOSED;
	p.skill_type = skill_type;
	SendPacket(&p);
}

void cSession::SendChaserWinPacket()
{
	SC_CHASER_WIN_PACKET p;
	p.size = sizeof(SC_CHASER_WIN_PACKET);
	p.type = SC_CHASER_WIN;
	SendPacketAndClose(&p);
}

void cSession::SendPickUpInkPacket(int c_id)
{
	SC_PICK_UP_INK_PACKET p;
	p.size = sizeof(SC_PICK_UP_INK_PACKET);
	p.type = SC_PICK_UP_INK;
	p.c_id = c_id;
	SendPacket(&p);
}

void cSession::SendPickUpStunPacket(int c_id)
{
	SC_PICK_UP_STUN_PACKET p;
	p.size = sizeof(SC_PICK_UP_STUN_PACKET);
	p.type = SC_PICK_UP_STUN;
	p.c_id = c_id;
	SendPacket(&p);
}

void cSession::SendPickUpExplosionPacket(int c_id)
{
	SC_PICK_UP_EXPLOSION_PACKET p;
	p.size = sizeof(SC_PICK_UP_EXPLOSION_PACKET);
	p.type = SC_PICK_UP_EXPLOSION;
	p.c_id = c_id;
	SendPacket(&p);
}
