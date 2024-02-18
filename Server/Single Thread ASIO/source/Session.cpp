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
				clients[igmd._player_ids[0]]->Set_Ingame_Num(roomNum);

				cIngameData data2;
				data2.SetRoomNumber(roomNum);
				data2.SetPosition(-2427.765165, 2498.606435, 100);
				data2.SetRadian(10);
				data2.SetHp(200);
				data2.SetRole(clients[igmd._player_ids[1]]->_charactor_num);
				data2.SetUserName(clients[igmd._player_ids[1]]->Get_User_Name());
				data2.SetMyClientNumber(igmd._player_ids[1]);
				data2.SetMyIngameNum(roomNum + 1);
				IngameDataList[data2.GetMyIngameNumber()] = data2;
				clients[igmd._player_ids[1]]->Set_Ingame_Num(roomNum+1);
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

		for (int id : IngameMapDataList[IngameDataList[c_ingame_id].GetRoomNumber()]._player_ids){
			if (id == -1)
				continue;
			clients[id]->Send_Move_Packet(c_id);
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


int cSession::Get_Ingame_Num()
{
	return _ingame_num;
}

void cSession::Set_Ingame_Num(int num)
{
	_ingame_num = num;
}
