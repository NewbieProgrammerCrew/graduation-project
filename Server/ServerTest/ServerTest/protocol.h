#pragma once
#define ID_LEN 10
#define PWD_LEN 10
#define NICKNAME_LEN 10
#define ROLE_LEN 8


constexpr int PORT_NUM = 8080;
constexpr int BUF_SIZE = 512;
constexpr int PROTOCOL_NAME_SIZE = 20;

constexpr int W_WIDTH = 8;
constexpr int W_HEIGHT = 8;

// Packet ID
constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;
constexpr char CS_ATTACK = 2;
constexpr char CS_HITTED = 3;
constexpr char CS_MAP_LOADED = 4;
constexpr char CS_SIGNUP = 5;
constexpr char CS_ROLE = 6;
constexpr char CS_PICKUP = 7;

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
constexpr char SC_PICKUP = 13;

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
};

struct CS_ATTACK_PACKET {			// 플레이어 때림
	unsigned char	size;
	char			type;
	int				id;
	float			ry;
	float			x, y, z;
};

struct CS_HITTED_PACKET {			// 플레이어 맞음
    unsigned char	size;
    char			type;
    int				id;
    float			hp;
};

struct CS_ITEM_PICKUP_PACKET {		// 플레이어 아이템 얻음
    unsigned char	size;
    char			type;
    int				id;
    int 			itemId;
    int 			itemType;

};

struct CS_MAP_LOADED_PACKET {		// 클라이언트 map 로드 완료
	unsigned char size;
	unsigned char type;
};


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
    //float  ry;
};
struct SC_PICKUP_PACKET {			// 플레이어 아이템 얻음
    unsigned char	size;
    char			type;
    int				id;  
    int				itemId;  
};


struct SC_SIGNUP_PACKET {			// 화원가입 실패 혹은 성공
	unsigned char	size;
	char			type;
	bool			success;
	int				errorCode;
	int				id;
};

#pragma pack (pop)