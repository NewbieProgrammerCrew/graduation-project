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
struct CS_LOGIN_PACKET {			// 로그??
	unsigned char	size;
	char			type;
	char			id[ID_LEN];
	char			password[PWD_LEN];
};

struct CS_SIGNUP_PACKET {			// ?�원가??
	unsigned char	size;
	char			type;
	char			id[ID_LEN];
	char			password[PWD_LEN];
	char			userName[NICKNAME_LEN];
};

struct CS_ROLE_PACKET {			// ??�� ?�송
	unsigned char	size;
	char			type;
	char			role[ROLE_LEN];
	int				charactorNum;		// 1~5 ?�존?? 6~7 ?�인�?
};

struct CS_MOVE_PACKET {				// ?�레?�어 ?�직임
	unsigned char	size;
	char			type;
	float			rx, ry, rz;
	float			x, y, z;
	float			speed;
	bool			jump;
};

struct CS_ATTACK_PACKET {			// ?�레?�어 ?�림 ?�니메이??
	unsigned char	size;
	char			type;
	float			rx,ry,rz;
	float			x, y, z;
};

struct CS_HIT_PACKET {			// ?�레?�어 ?��?지 처리
	unsigned char	size;
	char			type;
	float			rx, ry, rz;
	float			x, y, z;
};

struct CS_PICKUP_FUSE_PACKET {		// ?�레?�어 ?�이???�음
    unsigned char	size;
    char			type;
    int 			fuseIndex;			// 몇번�??�덱?�의 ?�즈?��?
};

struct CS_PICKUP_GUN_PACKET {		// ?�레?�어 총을 ?�음
	unsigned char	size;
	char			type;
	int 			gunType;		// ?�떤 총인지 -> 0 : 기절, 1 : ??��, 2 : 먹물
	int				itemBoxIndex;		// ?�떤 ?�자?�서 ?�었?��?
};

struct CS_USE_GUN_PACKET {		// ?�레?��? 총을 ?�용??
	unsigned char	size;
	char			type;
};

struct CS_MAP_LOADED_PACKET {		// ?�라?�언??map 로드 ?�료
	unsigned char size;
	char type;
};

struct CS_PUT_FUSE_PACKET {		// ?�즈�?박스???��?
	unsigned char	size;
	char			type;
	int				fuseBoxIndex;
};

struct CS_OPEN_ITEM_BOX_PACKET {		// ?�이??박스�??�음
	unsigned char	size;
	char			type;
	int				ItemBoxIndex;
};

struct CS_REMOVE_JELLY_PACKET {		// ?�레?�어가 ?�리�?부??
	unsigned char	size;
	char			type;
	int				jellyIndex;
};

struct CS_AIM_STATE_PACKET {		// ?�레?�어가 조�?????
	unsigned char	size;
	char			type;
};

struct CS_IDLE_STATE_PACKET {		// ?�레?�어가 ???�태�??�음.
	unsigned char	size;
	char			type;
};

struct CS_PRESS_F_PACKET {
	unsigned char	size;
	char			type;
	int				item;			// 0 defalt (?�무것도 ?�호?�용 ?�할??, 1 ?�자, 2 ?�즈박스 ?�물?? 3 ?�즈 ?�우�? 4 �? 
	int				index;			// ?�이?�의 ?�덱??
};

struct CS_RELEASE_F_PACKET {
	unsigned char	size;
	char			type;
	int				item;			// 0 defalt (?�무것도 ?�호?�용 ?�할??, 1 ?�자, 2 ?�즈박스 ?�물??
	int				index;
};

struct CS_CHASER_HITTED_PACKET {
	unsigned char	size;
	char			type;
	int				chaserID;		// ?�래??id
};

struct CS_RESET_FUSE_BOX_PACKET {
	unsigned char	size;
	char			type;
	int				index;		// ?�즈 박스???�덱??
};

struct CS_ESCAPE_PACKET {		// ?�출?�을??보내�?
	unsigned char	size;
	char			type;
};

struct CS_GO_TO_SCORE_PAGE_PACKET {		// 죽�? ?�레?�나 ?�출???�레?�어가 결과�?보기 버튼 ?�릭??보내�?
	unsigned char	size;
	char			type;
};

struct CS_EXIT_SCORE_PAGE_PACKET {	// ?�수?�이지?�서 메인메뉴�??�갈??보내�?
	unsigned char	size;
	char			type;
};

// ======================================================================================================

struct SC_LOGIN_INFO_PACKET {		// 로그???�보
	unsigned char	size;
	char			type;
	char			userName[NICKNAME_LEN];
	int				money;
	int				id;
};

struct SC_LOGIN_FAIL_PACKET {		// 로그???�패
	unsigned char	size;
	char			type;
	int				errorCode;
	int				id;
};

struct SC_MAP_INFO_PACKET {		// �??�보 ?�달
	unsigned char	size;
	char			type;
	int				mapid;
	int				patternid;
	int				fusebox[8];
	int				fusebox_color[8];
};

struct SC_ADD_PLAYER_PACKET {		// ?�레?�어 추�?
	unsigned char	size;
	char			type;
	int				id;
	int				_hp;
	float			x, y, z;
	char			role[PROTOCOL_NAME_SIZE];
	int				charactorNum;		// 1~5 ?�존?? 6~7 ?�인�?
};

struct SC_REMOVE_PLAYER_PACKET {	// ?�레?�어 ??��
	unsigned char	size;
	char			type;
	int				id;
};

struct SC_MOVE_PLAYER_PACKET {		// ?�레?�어 ?�직임
	unsigned char	size;
	char			type;
	int				id;
	float			x, y, z;
	float			rx, ry, rz;
	float			speed;
	bool			jump;
};

struct SC_HITTED_PACKET {			// ?�레?�어 맞음
	unsigned char   size;
	char            type;
	int             id;
	int             _hp;
};
struct SC_DEAD_PACKET {				// ?�레?�어 죽음
	unsigned char   size;
	char            type;
	int             id;
	int             _hp;
};
struct SC_ATTACK_PLAYER_PACKET {	// ?�레?�어 공격
    unsigned char	size;
    char			type;
    int				id;
    float			x, y, z;
    float			ry;
};
struct SC_PICKUP_FUSE_PACKET {			// ?�레?��? ?�즈�??�음
    unsigned char	size;
    char			type;
	int				id;			// ?�즈�??��? ?�레?�어 ?�이??
    int				index;		// ?��? ?�즈???�덱??
};
struct SC_PICKUP_GUN_PACKET {			// ?�레?��? 총을 ?�음
	unsigned char	size;
	char			type;
	int				id;			// 총을 ?��? ?�레?�어 ?�이??
	int				gun_type;	// ?��? 총의 ?�??
	int				itemBoxIndex;	// 총을 ?��? ?�자???�덱??
	int				leftGunType;	// 만약 ?��? 총을 가지�??�었?�면 ?��? 가지�??�던 총의 ?�덱?? ?�으�?-1
};
struct SC_USE_GUN_PACKET {			// ?�레?��? 총을 ?�용??
	unsigned char	size;
	char			type;
	int				id;			// 총을 ?�용???�레?�어 ?�이??
};

struct SC_SIGNUP_PACKET {			// ?�원가???�패 ?��? ?�공
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

struct SC_REMOVE_JELLY_PACKET {		// ?�레?�어가 ?�리�?부??
	unsigned char	size;
	char			type;
	int				jellyIndex;
};

struct SC_AIM_STATE_PACKET {		// ?�레?�어가 조�?????
	unsigned char	size;
	char			type;
	int				id;
};

struct SC_IDLE_STATE_PACKET {		// ?�레?�어가 ???�태�??�음
	unsigned char	size;
	char			type;
	int				id;
};
struct SC_UNLOCKING_FUSE_BOX_PAKCET {		// ?�레?�어가 ?�즈 ?�자�??�는중임
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
	int				id;				// ?��? ?�호?�용??멈췄?��?
	int				item;			// ?�떤 ?�이?�과 ?�호?�용?�고 ?�었?��?, 1 : ?�자, 2 : ?�즈박스 ?�물??
	int				index;			// �??�이?�의 ?�덱??
	float			progress;		// ?�재까�? 진행 ?�황
};

struct SC_CHASER_RESURRECTION_PACKET {
	unsigned char	size;
	char			type;
	int				id;				// ?��? 부?�하?��?
	float			x, y, z;		// 부???�치
	float			rx, ry, rz;		// 부??방향
	int				hp;				// 부??체력
};

struct SC_RESET_FUSE_BOX_PACKET {
	unsigned char	size;
	char			type;
	int				chaserId;			// ?�래??id
	int				index;		// ?�즈 박스???�덱??
};

struct SC_ESCAPE_PACKET {		// ?�구?�도 ?�탈???�해 ?�출?�면 가???�킷
	unsigned char	size;
	char			type;
	int				id;			// ?��? ?�출 ?�는지
};

struct SC_GAME_RESULT_PACKET {
	unsigned char	size;
	char			type;
	int				score;
};

#pragma pack (pop)