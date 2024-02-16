#include "Global.h"
#include "Session.h"
#include "FuseBox.h"
#include "Protocol.h"

mutex Mutex;

concurrency::concurrent_queue<int> AvailableUserIDs;

atomic_int NowUserNum;

//array <FuseBox, MAX_FUSE_BOX_NUM> FuseBoxes;

unordered_map<int, unordered_map<int, vector<Object>>> OBJS;		// 맵 번호 , 구역 , 객체들
array <FuseBox, MAX_FUSE_BOX_NUM> FuseBoxes;						// 퓨즈 박스 위치 정보

int MapId;

struct Ingame{
	bool			in_use = false;
	cIngameData*	ingame_ptr;
};

array<Ingame, 10000> Ingames;

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

struct Circle {
	float x;
	float y;
	float z;
	float r;
};

int get_new_client_id()
{
	for (int i = 0; i < MAX_USER; ++i)
		if (clients[i]->_in_use == false)
			return i;
	return -1;
}

float angleBetween(const Vector3D& v1, const Vector3D& v2) {
	float dotProduct = v1.dot(v2);
	float magnitudeProduct = v1.magnitude() * v2.magnitude();
	return acos(dotProduct / magnitudeProduct) * (180.0 / PI);  // Radians to degrees
}

Vector3D yawToDirectionVector(float yawDegrees) {
	float yawRadians = yawDegrees * (PI / 180.0f);
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
				Ingames[clients[c_id]->Get_Ingame_Num()].ingame_ptr->ColArea.push_back(x + y * 16);
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

int Get_New_ClientID()
{
	if (NowUserNum >= MAX_USER) {
		cout << "SERVER_FULL\n";
		exit(-1);
	}

	int newUserID;
	for (;;)
		if (AvailableUserIDs.try_pop(newUserID))
			break;
	NowUserNum++;
	return newUserID;
}

class cServer
{
private:
	tcp::acceptor	acceptor;
	tcp::socket		socket;
	void do_Accept()
	{
		acceptor.async_accept(socket, [this](boost::system::error_code ec) {
			if (!ec)
			{
				int p_id = Get_New_ClientID();
				clients[p_id] = std::make_shared<cSession>(std::move(socket), p_id);
				clients[p_id]->Start();
				do_Accept();
			}
			});
	}

public:
	cServer(boost::asio::io_context& io_service, int port) : acceptor(io_service, tcp::endpoint(tcp::v4(), port)), socket(io_service)
	{
		do_Accept();
	}
};

void Worker_Thread(boost::asio::io_context* service)
{
	service->run();
}

void Init_Server()
{
	for (int i = 0; i < MAX_USER; ++i) {
		AvailableUserIDs.push(i);
	}
}

// 충돌처리를 위한 구조체
double dotProduct(const Vector2D& v1, const Vector2D& v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

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
	Vector2D axes[] = { {1, 0}, {0, 1} };  // x축 y축
	for (const Vector2D& axis : axes) {
		if (areRectanglesSeparated(rectangle1, rectangle2, axis)) {
			return false;  // Separation found, no collision
		}
	}

	return true;  // No separation along any axis, collision detected
}

// 충돌 구역에 따라 객체들을 저장하는 함수
void add_colldata(Object obj) {
	rectangle rec1;
	rectangle rec2 = { {obj._pos_x, obj._pos_y}, obj._extent_x, obj._extent_y, obj._yaw * std::numbers::pi / 180.0 };
	for (int x = 0; x < ceil(float(MAP_X) / COL_SECTOR_SIZE); ++x) {
		for (int y = 0; y < ceil(float(MAP_Y) / COL_SECTOR_SIZE); ++y) {
			rec1 = { {-(MAP_X / 2) + float(x) * 800 + 400,-(MAP_Y / 2) + float(y) * 800 + 400}, 400, 400, 0 };
			if (areRectanglesColliding(rec1, rec2)) {
				OBJS[obj._map_num][x + y * 16].push_back(obj);
			}
		}
	}
}

// 객체 초기화 함수
int InIt_Objects() {
	for (int mapNum = 1; mapNum < MAX_MAP_NUM + 1; ++mapNum) {
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
				object._obj_name = it->name.GetString();
				const rapidjson::Value& dataArray = it->value;
				for (const auto& data : dataArray.GetArray()) {
					object._in_use = true;
					object._type = data["Type"].GetInt();
					object._pos_x = data["LocationX"].GetFloat();
					object._pos_y = data["LocationY"].GetFloat();
					object._pos_z = data["LocationZ"].GetFloat();
					object._extent_x = data["ExtentX"].GetFloat();
					object._extent_y = data["ExtentY"].GetFloat();
					object._extent_z = data["ExtentZ"].GetFloat();
					object._yaw = data["Yaw"].GetFloat();
					object._roll = data["Roll"].GetFloat();
					object._pitch = data["Pitch"].GetFloat();
					object._map_num = mapNum;

					add_colldata(object);
					//OBJS[mapNum][i++] = object;
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

	for (int mapNum = 1; mapNum < MAX_MAP_NUM + 1; ++mapNum) {
		char filePath[100];
		if (mapNum == 1)
			std::sprintf(filePath, "..\\..\\coll_data\\Stage%dFuseBoxCollision.json", mapNum);
		/*else if (mapNum == 2)
			std::sprintf(filePath, "..\\..\\coll_data\\Stage%dCollision.json", mapNum);
		else if (mapNum == 3)
			std::sprintf(filePath, "..\\..\\coll_data\\Stage%dCollision.json", mapNum);*/


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

		for (int mapNum = 1; mapNum < MAX_MAP_NUM + 1; ++mapNum) {
			char filePath[100];
			if (mapNum == 1)
				std::sprintf(filePath, "..\\..\\coll_data\\Stage%dJelly.json", mapNum);
			/*else if (mapNum == 2)
				std::sprintf(filePath, "..\\..\\coll_data\\Stage%dCollision.json", mapNum);
			else if (mapNum == 3)
				std::sprintf(filePath, "..\\..\\coll_data\\Stage%dCollision.json", mapNum);*/


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
	// 서버 준비
	cout << "맵 객체들 읽어오는중" << endl;
	if (InIt_Objects()) {
		cout << "충돌체크 파일 읽어오기 실패" << endl;
		return 1;
	}
	/*for (int i = 0; i < MAX_FUSE_BOX_NUM; ++i)
		FuseBoxes[i].index = i;*/					// 퓨즈 박스도 일단 보류
	cout << "맵 객체 읽기 완료" << endl;
	//thread timerThread(do_timer);      타이머는 ASIO 방식으로 바꾸기
	cout << "서버 시작" << endl;
	// 서버 시작
	boost::asio::io_context ioService;

	Init_Server();

	cServer server(ioService, PORT_NUM);

	Worker_Thread(& ioService);
}