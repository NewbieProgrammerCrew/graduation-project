#pragma once
#define ID_LEN 10
#define PWD_LEN 10
#define NICKNAME_LEN 10
#define ROLE_LEN 8
#define MAX_MAP_NUM 3
#define MAX_FUSE_NUM 8
#define MAX_FUSE_BOX_NUM 16


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
constexpr char CS_PICKUP = 7;
constexpr char CS_PUT_FUSE = 8;


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
constexpr char SC_FUSE_BOX_ACTIVE= 14;
constexpr char SC_HALF_PORTAL_GAUGE = 15;
constexpr char SC_MAX_PORTAL_GAUGE = 16;

#pragma pack (push, 1)	
struct CS_LOGIN_PACKET {			// �α���
	unsigned char	size;
	char			type;
	char			id[ID_LEN];
	char			password[PWD_LEN];
};

struct CS_SIGNUP_PACKET {			// ȸ������
	unsigned char	size;
	char			type;
	char			id[ID_LEN];
	char			password[PWD_LEN];
	char			userName[NICKNAME_LEN];
};

struct CS_ROLE_PACKET {			// ���� ����
	unsigned char	size;
	char			type;
	char			role[ROLE_LEN];
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
	int				id;
	float			rx,ry,rz;
	float			x, y, z;
};

struct CS_HIT_PACKET {			// �÷��̾� ������ ó��
	unsigned char	size;
	char			type;
	float			rx, ry, rz;
	float			x, y, z;
};

struct CS_ITEM_PICKUP_PACKET {		// �÷��̾� ������ ����
    unsigned char	size;
    char			type;
    int				id;
    int 			itemId;
    int 			itemType;

};

struct CS_MAP_LOADED_PACKET {		// Ŭ���̾�Ʈ map �ε� �Ϸ�
	unsigned char size;
	char type;
};

struct CS_PUT_FUSE_PACKET {		// Ŭ���̾�Ʈ map �ε� �Ϸ�
	unsigned char	size;
	char			type;
	int				fuseBoxIndex;
};

// ======================================================================================================

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
	int				pb1, pb2, pb3, pb4, pb5, pb6, pb7, pb8;
};

struct SC_ADD_PLAYER_PACKET {		// �÷��̾� �߰�
	unsigned char	size;
	char			type;
	int				id;
	int				_hp;
	float			x, y, z;
	char			role[PROTOCOL_NAME_SIZE];
};

struct SC_REMOVE_PLAYER_PACKET {	// �÷��̾� ����
	unsigned char	size;
	char			type;
	int				id;
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
struct SC_ATTACK_PLAYER_PACKET {	// �÷��̾� ����
    unsigned char	size;
    char			type;
    int				id;
    float			x, y, z;
    float			ry;
};
struct SC_PICKUP_PACKET {			// �÷��̾� ������ ����
    unsigned char	size;
    char			type;
    int				id;  
    int				itemId;  
};


struct SC_SIGNUP_PACKET {			// ȭ������ ���� Ȥ�� ����
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
#pragma pack (pop)