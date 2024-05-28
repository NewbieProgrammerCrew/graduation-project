#pragma once

#define L_BUF_SIZE 1024
#define LOBBY_SERVER 9000

#define ADDRESS_LEN 20

constexpr char  GAME_SERVER_OPENED = 0;


//===================================================================
constexpr char  LOBBY_SERVER_CONNECTED = 0;


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

#pragma pack (pop)