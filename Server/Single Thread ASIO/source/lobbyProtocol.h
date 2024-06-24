#pragma once

#define L_BUF_SIZE 1024
#define LOBBY_SERVER 9000

#define ADDRESS_LEN 20
constexpr int MAX_CHASER_NUM = 1;
constexpr int MAX_RUNNER_NUM = 1;
constexpr int MAX_GAME_SERVER_THREAD = 8;

constexpr char  GAME_SERVER_OPENED = 0;


//===================================================================
constexpr char  LOBBY_SERVER_CONNECTED = 0;
constexpr char  CREATE_ROOM = 1;


//===============================================================================
#pragma pack (push, 1)	
struct GAME_SERVER_OPENED_PACKET {
	unsigned char	size;
	char			type;
	int				portNum;
	char			address[ADDRESS_LEN];
};


// ====================================== 서버 -> 클라 패킷 ==========================================
struct LOBBY_SERVER_CONNECTED_PACKET {
	unsigned char	size;
	char			type;
};

struct CREATE_ROOM_PACKET {
	unsigned char	size;
	char			type;
	int				chaser;
	int				runners[5];
};


#pragma pack (pop)