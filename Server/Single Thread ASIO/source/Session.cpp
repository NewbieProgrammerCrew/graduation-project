#include "Session.h"

using namespace std;

extern unordered_map<int, unordered_map<int, vector<Object>>> OBJS;		// 맵 번호 , 구역 , 객체들
concurrency::concurrent_unordered_map<int, shared_ptr<cSession>> clients;
concurrency::concurrent_unordered_map<std::string, array<std::string, 2>> UserInfo;
concurrency::concurrent_unordered_set<std::string> UserName;
extern concurrency::concurrent_queue<int> AvailableUserIDs;
extern atomic_int NowUserNum;
extern array <FuseBox, MAX_FUSE_BOX_NUM> FuseBoxes;						// 퓨즈 박스 위치 정보

extern concurrency::concurrent_queue<int> AvailableRoomNumber;

concurrency::concurrent_queue<int> ChaserQueue;
concurrency::concurrent_queue<int> RunnerQueue;
concurrency::concurrent_unordered_map<int, IngameMapData> IngameMapDataList;  
concurrency::concurrent_unordered_map<int, cIngameData>	IngameDataList;

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

		// 로컬 좌표계에서 충돌 검사
		bool collisionX = std::abs(localX) <= obj._extent_x + circle.r;
		bool collisionY = std::abs(localY) <= obj._extent_y + circle.r;

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
	int c_ingame_id = clients[c_id]->_ingame_num;
	int room_number = c_ingame_id / 5;
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

		if (UserInfo.find(p->id) != UserInfo.end()) {	// 중복되는 아이디가 있는지 확인
			signupPacket.success = false;
			signupPacket.errorCode = 100;
			cout << "이미 사용중인 아이디 입니다.\n";
			clients[c_id]->Send_Packet(&signupPacket);
			break;
		}

		if (UserName.find(p->userName) != UserName.end()) {	// 중복되는 닉네임이 있는지 확인.
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

		bool allPlayersReady = false; // 모든 플레이어가 준비?
		if (ChaserQueue.unsafe_size() >= 1) {
			if (RunnerQueue.unsafe_size() >= 1) {			// [수정] RunnerQueue는 나중에 수정하기
				allPlayersReady = true;
			}
		}

		if (allPlayersReady) {
			IngameMapData igmd;
			if (!ChaserQueue.try_pop(igmd._player_ids[0])) break;
			if (!RunnerQueue.try_pop(igmd._player_ids[1])) break;		// [수정] 나중에 4명 받을때 for문 돌려서 4개 다 받아오게 바꾸기
			int mapId = rand() % 3 + 1;				// 맵 랜덤으로 결정
			int patternid = rand() % 3 + 1;			// 퓨즈 박스 패턴 랜덤으로 결정
			int colors[4]{ 0,0,0,0 };				// 퓨즈 박스 색
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

				// 인게임에 필요한 데이터 받아오기
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
			igmd._map_num = 1;								// [수정] 맵이 추가되면 id 수정할 것

			SC_MAP_INFO_PACKET mapinfo_packet;
			mapinfo_packet.size = sizeof(mapinfo_packet);
			mapinfo_packet.type = SC_MAP_INFO;
			mapinfo_packet.mapid = 1;						// [수정] 맵이 추가되면  id 수정할것
			mapinfo_packet.patternid = patternid;
			for (int i = 0; i < 8; ++i) {
				mapinfo_packet.fusebox[i] = igmd._fuse_box_list[i];
				mapinfo_packet.fusebox_color[i] = fuseBoxColorList[i];
			}
			int roomNum;							// 매칭 잡힌 클라이언트들에게 가능한 방 번호 부여
			AvailableRoomNumber.try_pop(roomNum);
			IngameMapDataList[roomNum] = igmd;
			for (int id : igmd._player_ids) {
				if (id == -1)
					continue;
				clients[id]->Send_Map_Info_Packet(mapinfo_packet);
			}
			
			// [수정] 클라에서 맵 로드하는 동안 서버에서 플레이어들 인게임 데이터 관리!! 지금은 하드코딩, 나중에 바꿀것.
			{
				//  술래 정보 저장
				cIngameData data;
				data.SetRoomNumber(roomNum);
				data.SetPosition(-2874.972553, -3263.0, 100);
				data.SetRadian(1);
				data.SetHp(600);
				data.SetRole(clients[igmd._player_ids[0]]->_charactor_num);
				data.SetUserName(clients[igmd._player_ids[0]]->Get_User_Name());
				data.SetMyClientNumber(igmd._player_ids[0]);
				data.SetMyIngameNum(roomNum);
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
				data2.SetMyIngameNum(roomNum + 1);
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
		bool allPlayersInMap = true; // 모든 플레이어가 준비?
		for (int id : igmd._player_ids) {
			if (id == -1)
				break;
			if (!clients[id]->_ingame) {
				allPlayersInMap = false;
				break;
			}
		}
		if (!allPlayersInMap) break;

		// 모든 플레이어가 준비되었으면, 모든 클라이언트에게 모든 플레이어 정보 전송
		cout << "모든 플레이어 맵 로드 완료\n";

		for (int m_id : igmd._player_ids) {
			if (m_id == -1)
				continue;

			for (int id : igmd._player_ids) {
				if (id == -1)
					continue;
				SC_ADD_PLAYER_PACKET app;
				app.size = sizeof(app);
				app.type = SC_ADD_PLAYER;
				app.id = clients[id]->Get_Ingame_Num();
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
		if (!CollisionTest(c_ingame_id, p->x, p->y, p->z, IngameDataList[c_ingame_id].GetRadian())) {
			IngameDataList[c_ingame_id].SetPosition(p->x, p->y, p->z);
		}
		else {
			static int num = 0;
			cout << c_id << " player in Wrong Place !" << num++ << endl;
		}

		IngameDataList[c_ingame_id].SetRotationValue(p->rx, p->ry, p->rz);
		IngameDataList[c_ingame_id].SetSeppd(p->speed);
		IngameDataList[c_ingame_id].SetJump(p->jump);

		for (int id : IngameMapDataList[room_number]._player_ids){
			if (id == -1)
				continue;
			clients[id]->Send_Move_Packet(c_id);
		}
		break;
	}

	case CS_ATTACK: {		// 때리는 모션 보여주기 위한 용도
		cout << "hitted!!!!!!!!!!!!!\n";
		CS_ATTACK_PACKET* p = reinterpret_cast<CS_ATTACK_PACKET*>(packet);
		IngameDataList[c_ingame_id].SetPosition(p->x, p->y, p->z);
		IngameDataList[c_ingame_id].SetRotationValue(p->rx, p->ry, p->rz);

		for (int id : IngameMapDataList[room_number]._player_ids) {
			if (id == -1) continue;
			clients[id]->Send_Attack_Packet(c_id);
		}

		Vector3D seekerDir = yawToDirectionVector(p->ry);
		Vector3D seekerPos{ p->x,p->y,p->z };

		cout << seekerDir.x << " " << seekerDir.y << " " << seekerDir.z << endl;

		for (int i = 0; i < 5; ++i) {
			if (!IngameDataList[room_number + i]._in_use) continue;
			if (room_number + i == c_ingame_id) continue;

			cIngameData igd = IngameDataList[room_number + i];

			Vector3D playerPos{ igd.GetPositionX(),igd.GetPositionY(), igd.GetPositionZ() };
			Vector3D directionToPlayer = playerPos - seekerPos;
			if (IngameDataList[c_ingame_id].GetRole() <= 5) {		// 만약 공격한사람이 생존자라면 ?
				const float MAX_SHOOTING_DISTANCE = 10000;  // 총 공격 사정거리 조절 필요
				const float SHOOTING_ANGLE = 45.f;
				if (directionToPlayer.magnitude() > MAX_SHOOTING_DISTANCE) {
					continue;
				}
				float angle = angleBetween(seekerDir.normalize(), directionToPlayer.normalize());
				if (angle <= SHOOTING_ANGLE) {
					int victimId = IngameDataList[room_number + i].GetMyClientNumber();
					IngameDataList[c_ingame_id].ChangeDamagenIflictedOnEnemy(200);
					IngameDataList[room_number + i].ChangeHp(-200);

					// 플레이어가 맞았다는 패킷 전송
					for (int id : IngameMapDataList[room_number]._player_ids) {
						if (id == -1) continue;
						clients[id]->Send_Other_Player_Hitted_Packet(victimId, IngameDataList[room_number + i].GetHp());
					}

					// 플레이어가 사망했으면 사망했다는 패킷도 전송
					if (IngameDataList[room_number + i].GetHp() <= 0) {
						for (int id : IngameMapDataList[room_number]._player_ids) {
							if (id == -1) continue;
							clients[id]->Send_Other_Player_Dead_Packet(victimId);
						}
						IngameDataList[room_number + i].SetDieState(true);
						break;
					}

				}
				else {
					std::cout << "시야 밖에 있습니다." << std::endl;
				}
			}

			else if (5 < IngameDataList[c_ingame_id].GetRole()) {
				const float CHASING_ANGLE = 45.f;
				const float MAX_CHASING_DISTANCE = 70.f;
				if (directionToPlayer.magnitude() > MAX_CHASING_DISTANCE) {
					continue;
				}

				float angle = angleBetween(seekerDir.normalize(), directionToPlayer.normalize());
				if (angle <= CHASING_ANGLE) {
					int victimId = IngameDataList[room_number + i].GetMyClientNumber();
					IngameDataList[c_ingame_id].ChangeDamagenIflictedOnEnemy(200);
					IngameDataList[room_number + i].ChangeHp(-200);

					// 플레이어가 맞았다는 패킷 전송
					for (int id : IngameMapDataList[room_number]._player_ids) {
						if (id == -1) continue;
						clients[id]->Send_Other_Player_Hitted_Packet(victimId, IngameDataList[room_number + i].GetHp());
					}

					// 플레이어가 사망했으면 사망했다는 패킷도 전송
					if (IngameDataList[room_number + i].GetHp() <= 0) {
						for (int id : IngameMapDataList[room_number]._player_ids) {
							if (id == -1) continue;
							clients[id]->Send_Other_Player_Dead_Packet(victimId);
						}
						IngameDataList[room_number + i].SetDieState(true);
						break;
					}
				}
				else {
					std::cout << "시야 밖에 있습니다." << std::endl;
				}
			}
		}
		break; 
	}

	case CS_PICKUP_FUSE: {
		cout << "send Pickup packet to client" << c_id << endl;
		if (5<(IngameDataList[c_ingame_id].GetRole()))
			break;
		cout << "send Pickup packet to client"  << c_id << endl;
		if (IngameDataList[c_ingame_id].GetFuseIndex() != -1)
			break;
		CS_PICKUP_FUSE_PACKET* p = reinterpret_cast<CS_PICKUP_FUSE_PACKET*>(packet);
		cout << "send Pickup packet to client"  << c_id << endl;
		if (IngameMapDataList[room_number]._fuses[p->fuseIndex].GetStatus() == AVAILABLE) {
			cout << "send Pickup packet to client" << c_id << endl;
			IngameMapDataList[room_number]._fuses[p->fuseIndex].SetStatus(ACQUIRED);
			IngameDataList[c_ingame_id].SetFuseIndex(p->fuseIndex);
			for (int id : IngameMapDataList[room_number]._player_ids) {
				if (id == -1)
					continue;
				clients[id]->Send_Pickup_Fuse_Packet(c_id, p->fuseIndex);
			}
		}
		break;
	}


	case 17:
		break;
	/*case CS_ATTACK: {
		CS_ATTACK_PACKET* p = reinterpret_cast<CS_ATTACK_PACKET*>(packet);
		clients[c_id].x = p->x;
		clients[c_id].y = p->y;
		clients[c_id].z = p->z;

		for (auto& pl : clients)
			if (true == pl.in_use)
				pl.send_attack_packet(c_id);
		break;

	}*/
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
			int index = 0;
			bool emptyRoom = true;
			// [수정] 만약 큐를 잡고 있던 상태였으면 큐에서 삭제할것 추가.
			for (int id : IngameMapDataList[IngameDataList[clients[_my_id]->Get_Ingame_Num()].GetRoomNumber()]._player_ids) {
				if (id == _my_id) {
					IngameMapDataList[IngameDataList[clients[_my_id]->Get_Ingame_Num()].GetRoomNumber()]._player_ids[index] = -1;
					break;
				}
				else if (id != -1) {
					emptyRoom = false;
				}
				index++;
			}
			if (emptyRoom == true) {
				AvailableRoomNumber.push(IngameDataList[clients[_my_id]->Get_Ingame_Num()].GetRoomNumber());
				IngameMapDataList.unsafe_erase(IngameDataList[clients[_my_id]->Get_Ingame_Num()].GetRoomNumber());
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
				// 패킷 조립
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

int cSession::Get_Ingame_Num()
{
	return _ingame_num;
}

void cSession::Set_Ingame_Num(int num)
{
	_ingame_num = num;
}
