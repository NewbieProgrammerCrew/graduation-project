#pragma once

#define BUF_SIZE 1024
#define PORT_NUM 8080
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



constexpr int PROTOCOL_NAME_SIZE = 20;
constexpr int MAX_OBJECTS = 100;


constexpr char CS_SIGNUP = 0;
constexpr char CS_LOGIN = 1;
constexpr char CS_ROLE = 2;
constexpr char CS_MAP_LOADED = 3;
constexpr char CS_MOVE = 4;
constexpr char CS_ATTACK = 5;
constexpr char CS_PICKUP_FUSE = 6;
constexpr char CS_PRESS_F = 7;
constexpr char CS_RELEASE_F = 8;



//===================================================================
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







//===============================================================================
#pragma pack (push, 1)	

struct CS_SIGNUP_PACKET {			// ȸ������
	unsigned char	size;
	char			type;
	char			id[ID_LEN];
	char			password[PWD_LEN];
	char			userName[NICKNAME_LEN];
};

struct CS_LOGIN_PACKET {			// �α���
	unsigned char	size;
	char			type;
	char			id[ID_LEN];
	char			password[PWD_LEN];
};

struct CS_ROLE_PACKET {			// ���� ����
	unsigned char	size;
	char			type;
	char			role[ROLE_LEN];
	int				charactorNum;		// 1~5 ������, 6~7 ���θ�
};

struct CS_MAP_LOADED_PACKET {		// Ŭ���̾�Ʈ map �ε� �Ϸ�
	unsigned char size;
	char type;
};

struct CS_MOVE_PACKET {				// �÷��̾� ������
	unsigned char	size;
	char			type;
	float			rx, ry, rz;
	float			x, y, z;
	float			speed;
	bool			jump;
};

struct CS_ATTACK_PACKET {			// �÷��̾� ���� �ִϸ��̼�
	unsigned char	size;
	char			type;
	float			rx, ry, rz;
	float			x, y, z;
};

struct CS_PICKUP_FUSE_PACKET {		// �÷��̾� ������ ����
	unsigned char	size;
	char			type;
	int 			fuseIndex;			// ���° �ε����� ǻ������
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
// ====================================== ���� -> Ŭ�� ��Ŷ ==========================================

struct SC_SIGNUP_PACKET {			// ȭ������ ���� Ȥ�� ����
	unsigned char	size;
	char			type;
	bool			success;
	int				errorCode;
	int				id;
};

struct SC_LOGIN_INFO_PACKET {		// �α��� ����
	unsigned char	size;
	char			type;
	char			userName[NICKNAME_LEN];
	int				money;
	int				id;
};

struct SC_LOGIN_FAIL_PACKET {		// �α��� ����
	unsigned char	size;
	char			type;
	int				errorCode;
	int				id;
};

struct SC_MAP_INFO_PACKET {		// �� ���� ����
	unsigned char	size;
	char			type;
	int				mapid;
	int				patternid;
	int				fusebox[8];
	int				fusebox_color[8];
};

struct SC_ADD_PLAYER_PACKET {		// �÷��̾� �߰�
	unsigned char	size;
	char			type;
	int				id;
	int				_hp;
	float			x, y, z;
	char			role[PROTOCOL_NAME_SIZE];
	int				charactorNum;				// 1~5 ������, 6~7 ���θ�
};

struct SC_MOVE_PLAYER_PACKET {		// �÷��̾� ������
	unsigned char	size;
	char			type;
	int				id;
	float			x, y, z;
	float			rx, ry, rz;
	float			speed;
	bool			jump;
};

struct SC_ATTACK_PLAYER_PACKET {	// �÷��̾� ���� ���
	unsigned char	size;
	char			type;
	int				id;
};

struct SC_HITTED_PACKET {			// �÷��̾� ����
	unsigned char   size;
	char            type;
	int             id;
	int             _hp;
};

struct SC_DEAD_PACKET {				// �÷��̾� ����
	unsigned char   size;
	char            type;
	int             id;
	int             _hp;
};

struct SC_PICKUP_FUSE_PACKET {			// �÷��̰� ǻ� ����
	unsigned char	size;
	char			type;
	int				id;			// ǻ� ���� �÷��̾� ���̵�
	int				index;		// ���� ǻ���� �ε���
};

struct SC_REMOVE_PLAYER_PACKET {	// �÷��̾� ����
	unsigned char	size;
	char			type;
	int				id;
};

struct SC_NOT_INTERACTIVE_PACKET {
	unsigned char	size;
	char			type;
};

struct SC_ITEM_BOX_OPENED_PACKET {
	unsigned char	size;
	char			type;
	int				index;
	int				gun_id;
};

struct SC_OPENING_ITEM_BOX_PACKET {
	unsigned char	size;
	char			type;
	int				id;
	int				index;
	float			progress;
};

struct SC_STOP_OPENING_PACKET {
	unsigned char	size;
	char			type;
	int				id;				// ?��? ?�호?�용??멈췄?��?
	int				item;			// ?�떤 ?�이?�과 ?�호?�용?�고 ?�었?��?, 1 : ?�자, 2 : ?�즈박스 ?�물??
	int				index;			// �??�이?�의 ?�덱??
	float			progress;		// ?�재까�? 진행 ?�황
};

#pragma pack (pop)