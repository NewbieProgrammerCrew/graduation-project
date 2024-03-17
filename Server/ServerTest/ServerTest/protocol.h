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
constexpr char CS_USE_GUN = 9;
constexpr char CS_PUT_FUSE = 10;
constexpr char CS_OPEN_ITEM_BOX = 11;
constexpr char CS_OPEN_FUSE_BOX = 12;
constexpr char CS_REMOVE_JELLY = 13;
constexpr char CS_AIM_STATE = 14;
constexpr char CS_IDLE_STATE = 15;
constexpr char CS_PICKUP_GUN = 16;
constexpr char CS_HIT = 17;
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
constexpr char SC_FUSE_BOX_OPENED = 24;
constexpr char SC_OPENING_ITEM_BOX = 12;
constexpr char SC_STOP_OPENING = 13;
constexpr char SC_PICKUP_GUN = 14;
constexpr char SC_USE_GUN = 15;
constexpr char SC_FUSE_BOX_ACTIVE= 16;
constexpr char SC_HALF_PORTAL_GAUGE = 17;
constexpr char SC_MAX_PORTAL_GAUGE = 18;
constexpr char SC_REMOVE_JELLY = 19;
constexpr char SC_AIM_STATE = 20;
constexpr char SC_IDLE_STATE = 21;
constexpr char SC_UNLOCKING_FUSE_BOX = 22;
constexpr char SC_CHANGE_HP = 23;
constexpr char SC_ITEM_BOX_OPENED = 11;
constexpr char SC_REMOVE_PLAYER = 25;
constexpr char SC_OPENING_FUSE_BOX = 26;
constexpr char SC_CHASER_RESURRECTION = 29;
constexpr char SC_RESET_FUSE_BOX = 30;
constexpr char SC_ESCAPE = 31;

#pragma pack (push, 1)	
struct CS_LOGIN_PACKET {			// ë¡œê·¸??
	unsigned char	size;
	char			type;
	char			id[ID_LEN];
	char			password[PWD_LEN];
};

struct CS_SIGNUP_PACKET {			// ?Œì›ê°€??
	unsigned char	size;
	char			type;
	char			id[ID_LEN];
	char			password[PWD_LEN];
	char			userName[NICKNAME_LEN];
};

struct CS_ROLE_PACKET {			// ??•  ?„ì†¡
	unsigned char	size;
	char			type;
	char			role[ROLE_LEN];
	int				charactorNum;		// 1~5 ?ì¡´?? 6~7 ?´ì¸ë§?
};

struct CS_MOVE_PACKET {				// ?Œë ˆ?´ì–´ ?€ì§ì„
	unsigned char	size;
	char			type;
	float			rx, ry, rz;
	float			x, y, z;
	float			speed;
	bool			jump;
};

struct CS_ATTACK_PACKET {			// ?Œë ˆ?´ì–´ ?Œë¦¼ ? ë‹ˆë©”ì´??
	unsigned char	size;
	char			type;
	float			rx,ry,rz;
	float			x, y, z;
};

struct CS_HIT_PACKET {			// ?Œë ˆ?´ì–´ ?°ë?ì§€ ì²˜ë¦¬
	unsigned char	size;
	char			type;
	float			rx, ry, rz;
	float			x, y, z;
};

struct CS_PICKUP_FUSE_PACKET {		// ?Œë ˆ?´ì–´ ?„ì´???»ìŒ
    unsigned char	size;
    char			type;
    int 			fuseIndex;			// ëª‡ë²ˆì§??¸ë±?¤ì˜ ?¨ì¦ˆ?¸ì?
};

struct CS_PICKUP_GUN_PACKET {		// ?Œë ˆ?´ì–´ ì´ì„ ?»ìŒ
	unsigned char	size;
	char			type;
	int 			gunType;		// ?´ë–¤ ì´ì¸ì§€ -> 0 : ê¸°ì ˆ, 1 : ??°œ, 2 : ë¨¹ë¬¼
	int				itemBoxIndex;		// ?´ë–¤ ?ì?ì„œ ?»ì—ˆ?”ì?
};

struct CS_USE_GUN_PACKET {		// ?Œë ˆ?´ê? ì´ì„ ?¬ìš©??
	unsigned char	size;
	char			type;
};

struct CS_MAP_LOADED_PACKET {		// ?´ë¼?´ì–¸??map ë¡œë“œ ?„ë£Œ
	unsigned char size;
	char type;
};

struct CS_PUT_FUSE_PACKET {		// ?¨ì¦ˆë¥?ë°•ìŠ¤???¼ì?
	unsigned char	size;
	char			type;
	int				fuseBoxIndex;
};

struct CS_OPEN_ITEM_BOX_PACKET {		// ?„ì´??ë°•ìŠ¤ë¥??´ìŒ
	unsigned char	size;
	char			type;
	int				ItemBoxIndex;
};

struct CS_REMOVE_JELLY_PACKET {		// ?Œë ˆ?´ì–´ê°€ ?¤ë¦¬ë¥?ë¶€??
	unsigned char	size;
	char			type;
	int				jellyIndex;
};

struct CS_AIM_STATE_PACKET {		// ?Œë ˆ?´ì–´ê°€ ì¡°ì?????
	unsigned char	size;
	char			type;
};

struct CS_IDLE_STATE_PACKET {		// ?Œë ˆ?´ì–´ê°€ ???íƒœë¡??ˆìŒ.
	unsigned char	size;
	char			type;
};

struct CS_PRESS_F_PACKET {
	unsigned char	size;
	char			type;
	int				item;			// 0 defalt (?„ë¬´ê²ƒë„ ?í˜¸?‘ìš© ?ˆí• ??, 1 ?ì, 2 ?¨ì¦ˆë°•ìŠ¤ ?ë¬¼?? 3 ?¨ì¦ˆ ?¼ìš°ê¸? 4 ì´? 
	int				index;			// ?„ì´?œì˜ ?¸ë±??
};

struct CS_RELEASE_F_PACKET {
	unsigned char	size;
	char			type;
	int				item;			// 0 defalt (?„ë¬´ê²ƒë„ ?í˜¸?‘ìš© ?ˆí• ??, 1 ?ì, 2 ?¨ì¦ˆë°•ìŠ¤ ?ë¬¼??
	int				index;
};

struct CS_CHASER_HITTED_PACKET {
	unsigned char	size;
	char			type;
	int				chaserID;		// ? ë˜??id
};

struct CS_RESET_FUSE_BOX_PACKET {
	unsigned char	size;
	char			type;
	int				index;		// ?¨ì¦ˆ ë°•ìŠ¤???¸ë±??
};

struct CS_ESCAPE_PACKET {		// ?ˆì¶œ?ˆì„??ë³´ë‚´ê¸?
	unsigned char	size;
	char			type;
};

struct CS_GO_TO_SCORE_PAGE_PACKET {		// ì£½ì? ?Œë ˆ?´ë‚˜ ?ˆì¶œ???Œë ˆ?´ì–´ê°€ ê²°ê³¼ì°?ë³´ê¸° ë²„íŠ¼ ?´ë¦­??ë³´ë‚´ê¸?
	unsigned char	size;
	char			type;
};

struct CS_EXIT_SCORE_PAGE_PACKET {	// ?ìˆ˜?˜ì´ì§€?ì„œ ë©”ì¸ë©”ë‰´ë¡??˜ê°ˆ??ë³´ë‚´ê¸?
	unsigned char	size;
	char			type;
};

// ======================================================================================================

struct SC_LOGIN_INFO_PACKET {		// ë¡œê·¸???•ë³´
	unsigned char	size;
	char			type;
	char			userName[NICKNAME_LEN];
	int				money;
	int				id;
};

struct SC_LOGIN_FAIL_PACKET {		// ë¡œê·¸???¤íŒ¨
	unsigned char	size;
	char			type;
	int				errorCode;
	int				id;
};

struct SC_MAP_INFO_PACKET {		// ë§??•ë³´ ?„ë‹¬
	unsigned char	size;
	char			type;
	int				mapid;
	int				patternid;
	int				fusebox[8];
	int				fusebox_color[8];
};

struct SC_ADD_PLAYER_PACKET {		// ?Œë ˆ?´ì–´ ì¶”ê?
	unsigned char	size;
	char			type;
	int				id;
	int				_hp;
	float			x, y, z;
	char			role[PROTOCOL_NAME_SIZE];
	int				charactorNum;		// 1~5 ?ì¡´?? 6~7 ?´ì¸ë§?
};

struct SC_REMOVE_PLAYER_PACKET {	// ?Œë ˆ?´ì–´ ?? œ
	unsigned char	size;
	char			type;
	int				id;
};

struct SC_MOVE_PLAYER_PACKET {		// ?Œë ˆ?´ì–´ ?€ì§ì„
	unsigned char	size;
	char			type;
	int				id;
	float			x, y, z;
	float			rx, ry, rz;
	float			speed;
	bool			jump;
};

struct SC_HITTED_PACKET {			// ?Œë ˆ?´ì–´ ë§ìŒ
	unsigned char   size;
	char            type;
	int             id;
	int             _hp;
};
struct SC_DEAD_PACKET {				// ?Œë ˆ?´ì–´ ì£½ìŒ
	unsigned char   size;
	char            type;
	int             id;
	int             _hp;
};
struct SC_ATTACK_PLAYER_PACKET {	// ?Œë ˆ?´ì–´ ê³µê²©
    unsigned char	size;
    char			type;
    int				id;
    float			x, y, z;
    float			ry;
};
struct SC_PICKUP_FUSE_PACKET {			// ?Œë ˆ?´ê? ?¨ì¦ˆë¥??»ìŒ
    unsigned char	size;
    char			type;
	int				id;			// ?¨ì¦ˆë¥??»ì? ?Œë ˆ?´ì–´ ?„ì´??
    int				index;		// ?»ì? ?¨ì¦ˆ???¸ë±??
};
struct SC_PICKUP_GUN_PACKET {			// ?Œë ˆ?´ê? ì´ì„ ?»ìŒ
	unsigned char	size;
	char			type;
	int				id;			// ì´ì„ ?»ì? ?Œë ˆ?´ì–´ ?„ì´??
	int				gun_type;	// ?»ì? ì´ì˜ ?€??
	int				itemBoxIndex;	// ì´ì„ ?»ì? ?ì???¸ë±??
	int				leftGunType;	// ë§Œì•½ ?´ê? ì´ì„ ê°€ì§€ê³??ˆì—ˆ?¤ë©´ ?´ê? ê°€ì§€ê³??ˆë˜ ì´ì˜ ?¸ë±?? ?†ìœ¼ë©?-1
};
struct SC_USE_GUN_PACKET {			// ?Œë ˆ?´ê? ì´ì„ ?¬ìš©??
	unsigned char	size;
	char			type;
	int				id;			// ì´ì„ ?¬ìš©???Œë ˆ?´ì–´ ?„ì´??
};

struct SC_SIGNUP_PACKET {			// ?”ì›ê°€???¤íŒ¨ ?¹ì? ?±ê³µ
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

struct SC_REMOVE_JELLY_PACKET {		// ?Œë ˆ?´ì–´ê°€ ?¤ë¦¬ë¥?ë¶€??
	unsigned char	size;
	char			type;
	int				jellyIndex;
};

struct SC_AIM_STATE_PACKET {		// ?Œë ˆ?´ì–´ê°€ ì¡°ì?????
	unsigned char	size;
	char			type;
	int				id;
};

struct SC_IDLE_STATE_PACKET {		// ?Œë ˆ?´ì–´ê°€ ???íƒœë¡??ˆìŒ
	unsigned char	size;
	char			type;
	int				id;
};
struct SC_UNLOCKING_FUSE_BOX_PAKCET {		// ?Œë ˆ?´ì–´ê°€ ?¨ì¦ˆ ?ìë¥??¬ëŠ”ì¤‘ì„
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
	int				gun_id;
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
	int				id;				// ?„ê? ?í˜¸?‘ìš©??ë©ˆì·„?”ì?
	int				item;			// ?´ë–¤ ?„ì´?œê³¼ ?í˜¸?‘ìš©?˜ê³  ?ˆì—ˆ?”ì?, 1 : ?ì, 2 : ?¨ì¦ˆë°•ìŠ¤ ?ë¬¼??
	int				index;			// ê·??„ì´?œì˜ ?¸ë±??
	float			progress;		// ?„ì¬ê¹Œì? ì§„í–‰ ?í™©
};

struct SC_CHASER_RESURRECTION_PACKET {
	unsigned char	size;
	char			type;
	int				id;				// ?„ê? ë¶€?œí•˜?”ì?
	float			x, y, z;		// ë¶€???„ì¹˜
	float			rx, ry, rz;		// ë¶€??ë°©í–¥
	int				hp;				// ë¶€??ì²´ë ¥
};

struct SC_RESET_FUSE_BOX_PACKET {
	unsigned char	size;
	char			type;
	int				chaserId;			// ? ë˜??id
	int				index;		// ?¨ì¦ˆ ë°•ìŠ¤???¸ë±??
};

struct SC_ESCAPE_PACKET {		// ?„êµ¬?¼ë„ ?¬íƒˆ???µí•´ ?ˆì¶œ?˜ë©´ ê°€???¨í‚·
	unsigned char	size;
	char			type;
	int				id;			// ?„ê? ?ˆì¶œ ?ˆëŠ”ì§€
};

struct SC_GAME_RESULT_PACKET {
	unsigned char	size;
	char			type;
	int				score;
};

#pragma pack (pop)