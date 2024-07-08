#include "Global.h"
#include "Session.h"
#include "FuseBox.h"
#include "Protocol.h"
#include "Jelly.h"

mutex Mutex;

concurrency::concurrent_queue<int> AvailableUserIDs;
concurrency::concurrent_queue<int> AvailableRoomNumber;

atomic_int NowUserNum;

unordered_map<int, unordered_map<int, vector<Object>>> OBJS;		// 맵 번호 , 구역 , 객체들
unordered_map<int,array <FuseBox, MAX_FUSE_BOX_NUM>> FuseBoxes;						// 퓨즈 박스 위치 정보
unordered_map<int,array<Jelly, MAX_JELLY_NUM>> Jellys;									// 젤리 위치 정보

int MapId;

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

struct Circle {
	double x;
	double y;
	double z;
	double r;
};

int get_new_client_id()
{
	for (int i = 0; i < MAX_USER; ++i)
		if (clients[i]->in_use_ == false)
			return i;
	return -1;
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
	for (int i = 0; i < 1000; ++i) {
		AvailableRoomNumber.push(i);
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
	rectangle rec2 = { {obj.pos_x_, obj.pos_y_}, obj.extent_x_, obj.extent_y_, obj.yaw_ * std::numbers::pi / 180.0 };
	for (int x = 0; x < ceil(double(MAP_X) / COL_SECTOR_SIZE); ++x) {
		for (int y = 0; y < ceil(double(MAP_Y) / COL_SECTOR_SIZE); ++y) {
			rec1 = { {-(MAP_X / 2) + double(x) * 800 + 400,-(MAP_Y / 2) + double(y) * 800 + 400}, 400, 400, 0 };
			if (areRectanglesColliding(rec1, rec2)) {
				OBJS[obj.map_num_][x + y * 16].push_back(obj);
			}
		}
	}
}

// 객체 초기화 함수
int InIt_Objects() {
	for (int mapNum = 1; mapNum <= MAX_MAP_NUM; ++mapNum) {
		char filePath[100];
		std::sprintf(filePath, "..\\coll_data\\Stage%dCollision.json", mapNum);


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
				object.obj_name_ = it->name.GetString();
				const rapidjson::Value& dataArray = it->value;
				for (const auto& data : dataArray.GetArray()) {
					object.in_use_ = true;
					object.type_ = data["Type"].GetInt();
					object.pos_x_ = data["LocationX"].GetDouble();
					object.pos_y_ = data["LocationY"].GetDouble();
					object.pos_z_ = data["LocationZ"].GetDouble();
					object.extent_x_ = data["ExtentX"].GetDouble();
					object.extent_y_ = data["ExtentY"].GetDouble();
					object.extent_z_ = data["ExtentZ"].GetDouble();
					object.yaw_ = data["Yaw"].GetDouble();
					object.roll_ = data["Roll"].GetDouble();
					object.pitch_ = data["Pitch"].GetDouble();
					object.map_num_ = mapNum;

					add_colldata(object);
				}
			}
		}
		else {
			std::cerr << "JSON parsing error." << std::endl;
		}
	}

	for (int mapNum = 1; mapNum <= MAX_MAP_NUM; ++mapNum) {
		char filePath[100];
		std::sprintf(filePath, "..\\coll_data\\Stage%dFuseBoxCollision.json", mapNum);

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
				fuseBox.obj_name_ = it->name.GetString();
				const rapidjson::Value& dataArray = it->value;
				for (const auto& data : dataArray.GetArray()) {
					fuseBox.type_ = data["Type"].GetInt();
					fuseBox.pos_x_ = data["LocationX"].GetDouble();
					fuseBox.pos_y_ = data["LocationY"].GetDouble();
					fuseBox.pos_z_ = data["LocationZ"].GetDouble();
					fuseBox.extent_x_ = data["ExtentX"].GetDouble();
					fuseBox.extent_y_= data["ExtentY"].GetDouble();
					fuseBox.extent_z_ = data["ExtentZ"].GetDouble();
					fuseBox.yaw_ = data["Yaw"].GetDouble();
					fuseBox.roll_ = data["Roll"].GetDouble();
					fuseBox.pitch_ = data["Pitch"].GetDouble();
					fuseBox.map_num_ = mapNum;
					FuseBoxes[mapNum - 1][data["index"].GetInt()] = fuseBox;
				}
			}
		}
		else {
			std::cerr << "JSON parsing error." << std::endl;
		}

		for (int mapNum = 1; mapNum <= MAX_MAP_NUM; ++mapNum) {
			char filePath[100];
			std::sprintf(filePath, "..\\coll_data\\Stage%dJelly.json", mapNum);

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
							data["LocationX"].GetDouble(),data["LocationY"].GetDouble(),data["LocationZ"].GetDouble(),
							data["ExtentX"].GetDouble(),data["ExtentY"].GetDouble(),data["ExtentZ"].GetDouble(),
							data["Yaw"].GetDouble(), data["Roll"].GetDouble(), data["Pitch"].GetDouble(),
							data["index"].GetInt()
						};
						Jellys[mapNum - 1][data["index"].GetInt()] = jelly;
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

void DoTimer(const boost::system::error_code& error, boost::asio::steady_timer* pTimer);
void DoBombTimer(const boost::system::error_code& error, boost::asio::steady_timer* pTimer);

boost::asio::io_context ioService;
boost::asio::steady_timer timer(ioService);
boost::asio::steady_timer bomb_timer(ioService);

int main()
{
	// init server
	cout << "맵 객체들 읽어오는중" << endl;
	if (InIt_Objects()) {
		cout << "충돌체크 파일 읽어오기 실패" << endl;
		return 1;
	}
	cout << "맵 객체 읽기 완료" << endl;

	timer.expires_from_now(boost::asio::chrono::milliseconds(100));
	timer.async_wait(boost::bind(DoTimer,boost::asio::placeholders::error,&timer));

	bomb_timer.expires_from_now(boost::asio::chrono::milliseconds(10));
	bomb_timer.async_wait(boost::bind(DoBombTimer, boost::asio::placeholders::error, &bomb_timer));;


	cout << "타이머 준비 완료" << endl;

	cout << "서버 시작" << endl;
	// 서버 시작

	Init_Server();

	cServer server(ioService, PORT_NUM);

	Worker_Thread(& ioService);
}