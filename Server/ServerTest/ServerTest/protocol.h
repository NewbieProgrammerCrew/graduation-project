#pragma once
#define ID_LEN 10
#define PWD_LEN 10
#define NICKNAME_LEN 10
#define ROLE_LEN 8
#define MAX_MAP_NUM 3
#define MAX_FUSE_NUM 8
#define MAX_FUSE_BOX_NUM 16
#define MAX_JELLY_NUM 20


constexpr int PORT_NUM = 8080;
constexpr int BUF_SIZE = 512;
constexpr int PROTOCOL_NAME_SIZE = 20;
constexpr int MAX_OBJECTS = 100;

constexpr int W_WIDTH = 8;
constexpr int W_HEIGHT = 8;

// Packet ID
constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;
constexpr char CS_ATTACK = 2;
constexpr char CS_HIT = 3;
constexpr char CS_MAP_LOADED = 4;
constexpr char CS_SIGNUP = 5;
constexpr char CS_ROLE = 6;
constexpr char CS_PICKUP_FUSE = 7;
constexpr char CS_PICKUP_GUN = 8;
constexpr char CS_USE_GUN = 9;
constexpr char CS_PUT_FUSE = 10;
constexpr char CS_OPEN_ITEM_BOX = 11;
constexpr char CS_OPEN_FUSE_BOX = 12;
constexpr char CS_REMOVE_JELLY = 13;
constexpr char CS_AIM_STATE = 14;
constexpr char CS_IDLE_STATE = 15;





constexpr char SC_LOGIN_INFO = 2;
constexpr char SC_LOGIN_FAIL = 3;
constexpr char SC_ADD_PLAYER = 4;
constexpr char SC_REMOVE_PLAYER = 5;
constexpr char SC_MOVE_PLAYER = 6;
constexpr char SC_CHANGE_HP = 7;
constexpr char SC_ATTACK_PLAYER = 8;
constexpr char SC_HITTED = 9;
constexpr char SC_DEAD = 10;
constexpr char SC_SIGNUP = 11;
constexpr char SC_MAP_INFO = 12;
constexpr char SC_PICKUP_FUSE = 13;
constexpr char SC_PICKUP_GUN = 14;
constexpr char SC_USE_GUN = 15;
constexpr char SC_FUSE_BOX_ACTIVE= 16;
constexpr char SC_HALF_PORTAL_GAUGE = 17;
constexpr char SC_MAX_PORTAL_GAUGE = 18;
constexpr char SC_REMOVE_JELLY = 19;
constexpr char SC_AIM_STATE = 20;
constexpr char SC_IDLE_STATE = 21;

#pragma pack (push, 1)	
struct CS_LOGIN_PACKET {			// 로그인
	unsigned char	size;
	char			type;
	char			id[ID_LEN];
	char			password[PWD_LEN];
};

struct CS_SIGNUP_PACKET {			// 회원가입
	unsigned char	size;
	char			type;
	char			id[ID_LEN];
	char			password[PWD_LEN];
	char			userName[NICKNAME_LEN];
};

struct CS_ROLE_PACKET {			// 역할 전송
	unsigned char	size;
	char			type;
	char			role[ROLE_LEN];
};

struct CS_MOVE_PACKET {				// 플레이어 움직임
	unsigned char	size;
	char			type;
	float			rx, ry, rz;
	float			x, y, z;
	float			speed;
	bool			jump;
};

struct CS_ATTACK_PACKET {			// 플레이어 때림 애니메이션
	unsigned char	size;
	char			type;
	int				id;
	float			rx,ry,rz;
	float			x, y, z;
};

struct CS_HIT_PACKET {			// 플레이어 데미지 처리
	unsigned char	size;
	char			type;
	float			rx, ry, rz;
	float			x, y, z;
};

struct CS_PICKUP_FUSE_PACKET {		// 플레이어 아이템 얻음
    unsigned char	size;
    char			type;
    int 			fuseIndex;			// 몇번째 인덱스의 퓨즈인지
};

struct CS_PICKUP_GUN_PACKET {		// 플레이어 총을 얻음
	unsigned char	size;
	char			type;
	int 			gunType;		// 어떤 총인지 -> 0 : 기절, 1 : 폭발, 2 : 먹물
};

struct CS_USE_GUN_PACKET {		// 플레이가 총을 사용함
	unsigned char	size;
	char			type;
};

struct CS_MAP_LOADED_PACKET {		// 클라이언트 map 로드 완료
	unsigned char size;
	char type;
};

struct CS_PUT_FUSE_PACKET {		// 퓨즈를 박스에 끼움
	unsigned char	size;
	char			type;
	int				fuseBoxIndex;
};

struct CS_OPEN_ITEM_BOX_PACKET {		// 아이템 박스를 열음
	unsigned char	size;
	char			type;
	int				ItemBoxIndex;
};

struct CS_REMOVE_JELLY_PACKET {		// 플레이어가 젤리를 부심
	unsigned char	size;
	char			type;
	int				jellyIndex;
};

struct CS_AIM_STATE_PACKET {		// 플레이어가 조준을 함
	unsigned char	size;
	char			type;
};

struct CS_IDLE_STATE_PACKET {		// 플레이어가 평 상태로 있음.
	unsigned char	size;
	char			type;
};
// ======================================================================================================

struct SC_LOGIN_INFO_PACKET {		// 로그인 정보
	unsigned char	size;
	char			type;
	char			userName[NICKNAME_LEN];
	int				money;
	int				id;
};

struct SC_LOGIN_FAIL_PACKET {		// 로그인 실패
	unsigned char	size;
	char			type;
	int				errorCode;
	int				id;
};

struct SC_MAP_INFO_PACKET {		// 맵 정보 전달
	unsigned char	size;
	char			type;
	int				mapid;
	int				patternid;
	int				fusebox[8];
	int				fusebox_color[8];
};

struct SC_ADD_PLAYER_PACKET {		// 플레이어 추가
	unsigned char	size;
	char			type;
	int				id;
	int				_hp;
	float			x, y, z;
	char			role[PROTOCOL_NAME_SIZE];
};

struct SC_REMOVE_PLAYER_PACKET {	// 플레이어 삭제
	unsigned char	size;
	char			type;
	int				id;
};

struct SC_MOVE_PLAYER_PACKET {		// 플레이어 움직임
	unsigned char	size;
	char			type;
	int				id;
	float			x, y, z;
	float			rx, ry, rz;
	float			speed;
	bool			jump;
};

struct SC_HITTED_PACKET {			// 플레이어 맞음
	unsigned char   size;
	char            type;
	int             id;
	int             _hp;
};
struct SC_DEAD_PACKET {				// 플레이어 죽음
	unsigned char   size;
	char            type;
	int             id;
	int             _hp;
};
struct SC_ATTACK_PLAYER_PACKET {	// 플레이어 공격
    unsigned char	size;
    char			type;
    int				id;
    float			x, y, z;
    float			ry;
};
struct SC_PICKUP_FUSE_PACKET {			// 플레이가 퓨즈를 얻음
    unsigned char	size;
    char			type;
	int				id;			// 퓨즈를 얻은 플레이어 아이디
    int				index;		// 얻은 퓨즈의 인덱스
};
struct SC_PICKUP_GUN_PACKET {			// 플레이가 총을 얻음
	unsigned char	size;
	char			type;
	int				id;			// 총을 얻은 플레이어 아이디
	int				gun_type;	// 얻은 총의 타입
};
struct SC_USE_GUN_PACKET {			// 플레이가 총을 사용함
	unsigned char	size;
	char			type;
	int				id;			// 총을 사용한 플레이어 아이디
};

struct SC_SIGNUP_PACKET {			// 화원가입 실패 혹은 성공
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

struct SC_REMOVE_JELLY_PACKET {		// 플레이어가 젤리를 부심
	unsigned char	size;
	char			type;
	int				jellyIndex;
};

struct SC_AIM_STATE_PACKET {		// 플레이어가 조준을 함
	unsigned char	size;
	char			type;
	int				id;
};

struct SC_IDLE_STATE_PACKET {		// 플레이어가 평 상태로 있음
	unsigned char	size;
	char			type;
	int				id;
};
#pragma pack (pop)