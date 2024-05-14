#pragma once
#define BUF_SIZE 1024
#define PORT_NUM 7777
#define MAX_USER 500
#define MAX_MAP_NUM 3
#define ROLE_LEN 8


#define COL_SECTOR_SIZE 800
#define MAP_X 12600
#define MAP_Y 12600
#define MAP_Z 7300
#define SEC_TO_MICRO 1000000

#define ID_LEN 10
#define PWD_LEN 10
#define NICKNAME_LEN 10
#define MAX_FUSE_BOX_NUM 16
#define MAX_JELLY_NUM 20
#define INGAME_MAX_FUSE_BOX_NUM 8
#define BOMB_SPEED 50

#define CHASER_HIT_RANGE 50


enum BombType { Stun, Explosion, Blind, NoBomb };

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
constexpr char CS_USE_SKILL = 14;
constexpr char CS_ESCAPE = 15;
constexpr char CS_OPEN_ITEM_BOX = 16;
constexpr char CS_OPEN_FUSE_BOX = 17;
constexpr char CS_RESET_FUSE_BOX = 18;
constexpr char CS_GO_TO_SCORE_PAGE = 19;
constexpr char CS_EXIT_SCORE_PAGE = 20;





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
constexpr char SC_CANNON_FIRE = 22;
constexpr char SC_BOMB_EXPLOSION = 23;
constexpr char SC_REMOVE_JELLY = 24;
constexpr char SC_USE_SKILL = 25;
constexpr char SC_CHASER_RESURRECTION = 26;
constexpr char SC_ESCAPE = 27;
constexpr char SC_REMOVE_PLAYER = 28;    
constexpr char SC_CHANGE_HP = 29;
constexpr char SC_USE_GUN = 30;
constexpr char SC_UNLOCKING_FUSE_BOX = 31;
constexpr char SC_RESET_FUSE_BOX = 32;

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
	double			rx, ry, rz;
	double			x, y, z;
	double			pitch;
	double			speed;
	bool			jump;
};

struct CS_ATTACK_PACKET {
	unsigned char	size;
	char			type;
	double			rx,ry,rz;
	double			x, y, z;
};

struct CS_HIT_PACKET {
	unsigned char	size;
	char			type;
	double			rx, ry, rz;
	double			x, y, z;
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
	double			x, y, z;
	double			rx, ry, rz;
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
struct CS_USE_SKILL_PACKET {
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
	double			x, y, z;
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
	double			x, y, z;
	double			rx, ry, rz;
	double			pitch;
	double			speed;
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
    double			x, y, z;
    double			ry;
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
	int				bombIndex;
};

struct SC_CANNON_FIRE_PACKET {
	unsigned char	size;
	char			type;
	int				id;
	int				bomb_index;
	double			x, y, z;
	double			rx, ry,rz;
	int				bomb_type;
};
struct SC_BOMB_EXPLOSION_PACKET {
	unsigned char	size;
	char			type;
	BombType		bomb_index;
};

struct SC_REMOVE_JELLY_PACKET {		// ?åÎ†à?¥Ïñ¥Í∞Ä ?§Î¶¨Î•?Î∂Ä??
	unsigned char	size;
	char			type;
	int				jellyIndex;
	float			b_x, b_y, b_z;
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
	double			progress;
};

struct SC_ITEM_BOX_OPENED_PACKET {
	unsigned char	size;
	char			type;
	int				index;
	BombType		bomb_type;
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
	double			progress;
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
	double			progress;		
};

struct SC_CHASER_RESURRECTION_PACKET {
	unsigned char	size;
	char			type;
	int				id;				
	double			x, y, z;		
	double			rx, ry, rz;		
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
	bool			die;
	int				score;
	bool			win;
};

struct SC_GAME_RESULT_PACKET {
	unsigned char	size;
	char			type;
	int				score;
};
struct SC_USE_SKILL_PACKET {
	unsigned char	size;
	char			type;
	int				id;
};

#pragma pack (pop)