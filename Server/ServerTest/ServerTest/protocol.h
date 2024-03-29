#pragma once
#define ID_LEN 10
#define PWD_LEN 10
#define NICKNAME_LEN 10
#define ROLE_LEN 8
#define MAX_MAP_NUM 3
#define MAX_FUSE_NUM 8
#define MAX_FUSE_BOX_NUM 16
#define MAX_JELLY_NUM 20
#define MAX_ITEM_BOX_NUM 10


constexpr int PORT_NUM = 8080;
constexpr int BUF_SIZE = 512;
constexpr int PROTOCOL_NAME_SIZE = 20;
constexpr int MAX_OBJECTS = 100;

constexpr int W_WIDTH = 8;
constexpr int W_HEIGHT = 8;

// Packet ID
constexpr char CS_SIGNUP = 0;
constexpr char CS_LOGIN = 1;
constexpr char CS_ROLE = 2;
constexpr char CS_MAP_LOADED = 3;
constexpr char CS_MOVE = 4;
constexpr char CS_ATTACK = 5;
constexpr char CS_PICKUP_FUSE = 6;
constexpr char CS_PRESS_F = 7;
constexpr char CS_RELEASE_F = 8;
constexpr char CS_PUT_FUSE = 9;
constexpr char CS_PICKUP_BOMB = 10;
constexpr char CS_AIM_STATE = 11;
constexpr char CS_IDLE_STATE = 12;
constexpr char CS_CANNON_FIRE = 13;
constexpr char CS_REMOVE_JELLY = 14;
constexpr char CS_OPEN_ITEM_BOX = 15;
constexpr char CS_OPEN_FUSE_BOX = 16;
constexpr char CS_PICKUP_GUN = 17;
constexpr char CS_BOX_OPEN = 18;
constexpr char CS_CHASER_HITTED = 19;
constexpr char CS_RESET_FUSE_BOX = 20;
constexpr char CS_ESCAPE = 21;
constexpr char CS_GO_TO_SCORE_PAGE = 22;
constexpr char CS_EXIT_SCORE_PAGE = 23;





constexpr char SC_SIGNUP = 0;
constexpr char SC_LOGIN_INFO = 1;
constexpr char SC_LOGIN_FAIL = 2;
constexpr char SC_MAP_INFO = 3;
constexpr char SC_ADD_PLAYER = 4;
constexpr char SC_MOVE_PLAYER = 5;
constexpr char SC_ATTACK_PLAYER = 6;
constexpr char SC_HITTED = 7;
constexpr char SC_DEAD = 8;
constexpr char SC_PICKUP_FUSE = 9;
constexpr char SC_NOT_INTERACTIVE = 10;
constexpr char SC_ITEM_BOX_OPENED = 11;
constexpr char SC_OPENING_ITEM_BOX = 12;
constexpr char SC_STOP_OPENING = 13;
constexpr char SC_OPENING_FUSE_BOX = 14;
constexpr char SC_FUSE_BOX_OPENED = 15;
constexpr char SC_FUSE_BOX_ACTIVE= 16;
constexpr char SC_HALF_PORTAL_GAUGE = 17;
constexpr char SC_MAX_PORTAL_GAUGE = 18;
constexpr char SC_PICKUP_BOMB = 19;
constexpr char SC_AIM_STATE = 20;
constexpr char SC_IDLE_STATE = 21;
constexpr char SC_REMOVE_JELLY = 22;
constexpr char SC_UNLOCKING_FUSE_BOX = 23;
constexpr char SC_CHANGE_HP = 24;
constexpr char SC_USE_GUN = 25;
constexpr char SC_REMOVE_PLAYER = 26;
constexpr char SC_CHASER_RESURRECTION = 27;
constexpr char SC_RESET_FUSE_BOX = 28;
constexpr char SC_ESCAPE = 29;

#pragma pack (push, 1)	
struct CS_LOGIN_PACKET {			// Î°úÍ∑∏??
	unsigned char	size;
	char			type;
	char			id[ID_LEN];
	char			password[PWD_LEN];
};

struct CS_SIGNUP_PACKET {			// ?åÏõêÍ∞Ä??
	unsigned char	size;
	char			type;
	char			id[ID_LEN];
	char			password[PWD_LEN];
	char			userName[NICKNAME_LEN];
};

struct CS_ROLE_PACKET {			// ??ï† ?ÑÏÜ°
	unsigned char	size;
	char			type;
	char			role[ROLE_LEN];
	int				charactorNum;		// 1~5 ?ùÏ°¥?? 6~7 ?¥Ïù∏Îß?
};

struct CS_MOVE_PACKET {				// ?åÎ†à?¥Ïñ¥ ?ÄÏßÅÏûÑ
	unsigned char	size;
	char			type;
	float			rx, ry, rz;
	float			x, y, z;
	float			speed;
	bool			jump;
};

struct CS_ATTACK_PACKET {
	unsigned char	size;
	char			type;
	float			rx,ry,rz;
	float			x, y, z;
};

struct CS_HIT_PACKET {
	unsigned char	size;
	char			type;
	float			rx, ry, rz;
	float			x, y, z;
};

struct CS_PICKUP_FUSE_PACKET {
    unsigned char	size;
    char			type;
    int 			fuseIndex;
};

struct CS_PICKUP_BOMB_PACKET {		
	unsigned char	size;
	char			type;
	int 			bombType;		
	int				itemBoxIndex;		
};

struct CS_CANNON_FIRE_PACKET {
	unsigned char	size;
	char			type;
	float			x, y, z;
	float			yaw, pitch;
};


struct CS_USE_GUN_PACKET {
	unsigned char	size;
	char			type;
};

struct CS_MAP_LOADED_PACKET {
	unsigned char size;
	char type;
};

struct CS_PUT_FUSE_PACKET {
	unsigned char	size;
	char			type;
	int				fuseBoxIndex;
};

struct CS_OPEN_ITEM_BOX_PACKET {
	unsigned char	size;
	char			type;
	int				ItemBoxIndex;
};

struct CS_REMOVE_JELLY_PACKET {
	unsigned char	size;
	char			type;
	int				jellyIndex;
};

struct CS_AIM_STATE_PACKET {
	unsigned char	size;
	char			type;
};

struct CS_IDLE_STATE_PACKET {
	unsigned char	size;
	char			type;
};

struct CS_PRESS_F_PACKET {
	unsigned char	size;
	char			type;
	int				item;		
	int				index;			
};

struct CS_RELEASE_F_PACKET {
	unsigned char	size;
	char			type;
	int				item;			
	int				index;
};

struct CS_CHASER_HITTED_PACKET {
	unsigned char	size;
	char			type;
	int				chaserID;		
};

struct CS_RESET_FUSE_BOX_PACKET {
	unsigned char	size;
	char			type;
	int				index;		
};

struct CS_ESCAPE_PACKET {		
	unsigned char	size;
	char			type;
};

struct CS_GO_TO_SCORE_PAGE_PACKET {		
	unsigned char	size;
	char			type;
};

struct CS_EXIT_SCORE_PAGE_PACKET {	
	unsigned char	size;
	char			type;
};

// ======================================================================================================

struct SC_LOGIN_INFO_PACKET {	
	unsigned char	size;
	char			type;
	char			userName[NICKNAME_LEN];
	int				money;
	int				id;
};

struct SC_LOGIN_FAIL_PACKET {		 
	unsigned char	size;
	char			type;
	int				errorCode;
	int				id;
};

struct SC_MAP_INFO_PACKET {		
	unsigned char	size;
	char			type;
	int				mapid;
	int				patternid;
	int				fusebox[8];
	int				fusebox_color[8];
};

struct SC_ADD_PLAYER_PACKET {		
	unsigned char	size;
	char			type;
	int				id;
	int				_hp;
	float			x, y, z;
	char			role[PROTOCOL_NAME_SIZE];
	int				charactorNum;		
};

struct SC_REMOVE_PLAYER_PACKET {	
	unsigned char	size;
	char			type;
	int				id;
};

struct SC_MOVE_PLAYER_PACKET {		
	unsigned char	size;
	char			type;
	int				id;
	float			x, y, z;
	float			rx, ry, rz;
	float			speed;
	bool			jump;
};

struct SC_HITTED_PACKET {			
	unsigned char   size;
	char            type;
	int             id;
	int             _hp;
};
struct SC_DEAD_PACKET {				
	unsigned char   size;
	char            type;
	int             id;
	int             _hp;
};
struct SC_ATTACK_PLAYER_PACKET {	
    unsigned char	size;
    char			type;
    int				id;
    float			x, y, z;
    float			ry;
};
struct SC_PICKUP_FUSE_PACKET {			
    unsigned char	size;
    char			type;
	int				id;			
    int				index;		
};
struct SC_PICKUP_BOMB_PACKET {
	unsigned char	size;
	char			type;
	int				id;
	int				bombType;
	int				itemBoxIndex;
	int				leftBombType;
};

struct SC_USE_GUN_PACKET {			
	unsigned char	size;
	char			type;
	int				id;			
};

struct SC_SIGNUP_PACKET {			
	unsigned char	size;
	char			type;
	bool			success;
	int				errorCode;
	int				id;
};
struct SC_FUSE_BOX_ACTIVE_PACKET {			
	unsigned char	size;
	char			type;
	int				fuseBoxIndex;
};
struct SC_HALF_PORTAL_GAUGE_PACKET {	
	unsigned char	size;
	char			type;
};
struct SC_MAX_PORTAL_GAUGE_PACKET {			
	unsigned char	size;
	char			type;
};

struct SC_REMOVE_JELLY_PACKET {	
	unsigned char	size;
	char			type;
	int				jellyIndex;
};

struct SC_AIM_STATE_PACKET {		
	unsigned char	size;
	char			type;
	int				id;
};

struct SC_IDLE_STATE_PACKET {		
	unsigned char	size;
	char			type;
	int				id;
};
struct SC_UNLOCKING_FUSE_BOX_PAKCET {		
	unsigned char	size;
	char			type;
	int				id;
};

struct SC_OPENING_ITEM_BOX_PACKET {
	unsigned char	size;
	char			type;
	int				id;
	int				index;
	float			progress;
};

struct SC_ITEM_BOX_OPENED_PACKET {
	unsigned char	size;
	char			type;
	int				index;
	int				bomb_type;
};

struct SC_NOT_INTERACTIVE_PACKET {
	unsigned char	size;
	char			type;
};

struct SC_OPENING_FUSE_BOX_PACKET {
	unsigned char	size;
	char			type;
	int				id;
	int				index;
	float			progress;
};

struct SC_FUSE_BOX_OPENED_PACKET {
	unsigned char	size;
	char			type;
	int				index;
};

struct SC_STOP_OPENING_PACKET {
	unsigned char	size;
	char			type;
	int				id;				
	int				item;			
	int				index;			
	float			progress;		
};

struct SC_CHASER_RESURRECTION_PACKET {
	unsigned char	size;
	char			type;
	int				id;				
	float			x, y, z;		
	float			rx, ry, rz;		
	int				hp;				
};

struct SC_RESET_FUSE_BOX_PACKET {
	unsigned char	size;
	char			type;
	int				chaserId;			
	int				index;		
};

struct SC_ESCAPE_PACKET {		
	unsigned char	size;
	char			type;
	int				id;		
};

struct SC_GAME_RESULT_PACKET {
	unsigned char	size;
	char			type;
	int				score;
};

#pragma pack (pop)