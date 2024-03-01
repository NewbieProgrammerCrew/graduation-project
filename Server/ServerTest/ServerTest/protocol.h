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
constexpr char CS_LOGIN = 1;
constexpr char CS_MOVE = 4;
constexpr char CS_ATTACK = 5;
constexpr char CS_HIT = 7;
constexpr char CS_MAP_LOADED = 3;
constexpr char CS_SIGNUP = 0;
constexpr char CS_ROLE = 2;
constexpr char CS_PICKUP_FUSE = 6;
constexpr char CS_PICKUP_GUN = 8;
constexpr char CS_USE_GUN = 9;
constexpr char CS_PUT_FUSE = 10;
constexpr char CS_OPEN_ITEM_BOX = 11;
constexpr char CS_OPEN_FUSE_BOX = 12;
constexpr char CS_REMOVE_JELLY = 13;
constexpr char CS_AIM_STATE = 14;
constexpr char CS_IDLE_STATE = 15;
constexpr char CS_PRESS_F = 16;
constexpr char CS_RELEASE_F = 17;
constexpr char CS_BOX_OPEN = 18;
constexpr char CS_CHASER_HITTED = 19;
constexpr char CS_RESET_FUSE_BOX = 20;
constexpr char CS_ESCAPE = 21;
constexpr char CS_GO_TO_SCORE_PAGE = 22;
constexpr char CS_EXIT_SCORE_PAGE = 23;





constexpr char SC_LOGIN_INFO = 1;
constexpr char SC_LOGIN_FAIL = 2;
constexpr char SC_ADD_PLAYER = 4;
constexpr char SC_MOVE_PLAYER = 5;
constexpr char SC_HITTED = 7;
constexpr char SC_DEAD = 8;
constexpr char SC_CHANGE_HP = 10;
// <<<<<<< HEAD
// constexpr char SC_REMOVE_PLAYER = 9;
// constexpr char SC_ATTACK_PLAYER = 6;
// =======
constexpr char SC_REMOVE_PLAYER = 13;
constexpr char SC_ATTACK_PLAYER = 8;

constexpr char SC_SIGNUP = 0;
constexpr char SC_MAP_INFO = 3;
constexpr char SC_PICKUP_FUSE = 9;
constexpr char SC_PICKUP_GUN = 14;
constexpr char SC_USE_GUN = 15;
constexpr char SC_FUSE_BOX_ACTIVE= 16;
constexpr char SC_HALF_PORTAL_GAUGE = 17;
constexpr char SC_MAX_PORTAL_GAUGE = 18;
constexpr char SC_REMOVE_JELLY = 19;
constexpr char SC_AIM_STATE = 20;
constexpr char SC_IDLE_STATE = 21;
constexpr char SC_UNLOCKING_FUSE_BOX = 22;
constexpr char SC_OPENING_ITEM_BOX = 23;
constexpr char SC_ITEM_BOX_OPENED = 24;
constexpr char SC_NOT_INTERACTIVE = 25;
constexpr char SC_OPENING_FUSE_BOX = 26;
constexpr char SC_FUSE_BOX_OPENED = 27;
constexpr char SC_STOP_OPENING = 28;
constexpr char SC_CHASER_RESURRECTION = 29;
constexpr char SC_RESET_FUSE_BOX = 30;
constexpr char SC_ESCAPE = 31;

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
	int				charactorNum;		// 1~5 ������, 6~7 ���θ�
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
	float			rx,ry,rz;
	float			x, y, z;
};

struct CS_HIT_PACKET {			// �÷��̾� ������ ó��
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

struct CS_PICKUP_GUN_PACKET {		// �÷��̾� ���� ����
	unsigned char	size;
	char			type;
	int 			gunType;		// � ������ -> 0 : ����, 1 : ����, 2 : �Թ�
	int				itemBoxIndex;		// � ���ڿ��� �������
};

struct CS_USE_GUN_PACKET {		// �÷��̰� ���� �����
	unsigned char	size;
	char			type;
};

struct CS_MAP_LOADED_PACKET {		// Ŭ���̾�Ʈ map �ε� �Ϸ�
	unsigned char size;
	char type;
};

struct CS_PUT_FUSE_PACKET {		// ǻ� �ڽ��� ����
	unsigned char	size;
	char			type;
	int				fuseBoxIndex;
};

struct CS_OPEN_ITEM_BOX_PACKET {		// ������ �ڽ��� ����
	unsigned char	size;
	char			type;
	int				ItemBoxIndex;
};

struct CS_REMOVE_JELLY_PACKET {		// �÷��̾ ������ �ν�
	unsigned char	size;
	char			type;
	int				jellyIndex;
};

struct CS_AIM_STATE_PACKET {		// �÷��̾ ������ ��
	unsigned char	size;
	char			type;
};

struct CS_IDLE_STATE_PACKET {		// �÷��̾ �� ���·� ����.
	unsigned char	size;
	char			type;
};

struct CS_PRESS_F_PACKET {
	unsigned char	size;
	char			type;
	int				item;			// 0 defalt (�ƹ��͵� ��ȣ�ۿ� ���Ҷ�), 1 ����, 2 ǻ��ڽ� �ڹ���, 3 ǻ�� �����, 4 ��  
	int				index;			// �������� �ε���
};

struct CS_RELEASE_F_PACKET {
	unsigned char	size;
	char			type;
	int				item;			// 0 defalt (�ƹ��͵� ��ȣ�ۿ� ���Ҷ�), 1 ����, 2 ǻ��ڽ� �ڹ���
	int				index;
};

struct CS_CHASER_HITTED_PACKET {
	unsigned char	size;
	char			type;
	int				chaserID;		// ������ id
};

struct CS_RESET_FUSE_BOX_PACKET {
	unsigned char	size;
	char			type;
	int				index;		// ǻ�� �ڽ��� �ε���
};

struct CS_ESCAPE_PACKET {		// Ż�������� ������
	unsigned char	size;
	char			type;
};

struct CS_GO_TO_SCORE_PAGE_PACKET {		// ���� �÷��̳� Ż���� �÷��̾ ���â ���� ��ư Ŭ���� ������
	unsigned char	size;
	char			type;
};

struct CS_EXIT_SCORE_PAGE_PACKET {	// �������������� ���θ޴��� ������ ������
	unsigned char	size;
	char			type;
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
	int				charactorNum;		// 1~5 ������, 6~7 ���θ�
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
struct SC_PICKUP_FUSE_PACKET {			// �÷��̰� ǻ� ����
    unsigned char	size;
    char			type;
	int				id;			// ǻ� ���� �÷��̾� ���̵�
    int				index;		// ���� ǻ���� �ε���
};
struct SC_PICKUP_GUN_PACKET {			// �÷��̰� ���� ����
	unsigned char	size;
	char			type;
	int				id;			// ���� ���� �÷��̾� ���̵�
	int				gun_type;	// ���� ���� Ÿ��
	int				itemBoxIndex;	// ���� ���� ������ �ε���
	int				leftGunType;	// ���� ���� ���� ������ �־��ٸ� ���� ������ �ִ� ���� �ε���, ������ -1
};
struct SC_USE_GUN_PACKET {			// �÷��̰� ���� �����
	unsigned char	size;
	char			type;
	int				id;			// ���� ����� �÷��̾� ���̵�
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

struct SC_REMOVE_JELLY_PACKET {		// �÷��̾ ������ �ν�
	unsigned char	size;
	char			type;
	int				jellyIndex;
};

struct SC_AIM_STATE_PACKET {		// �÷��̾ ������ ��
	unsigned char	size;
	char			type;
	int				id;
};

struct SC_IDLE_STATE_PACKET {		// �÷��̾ �� ���·� ����
	unsigned char	size;
	char			type;
	int				id;
};
struct SC_UNLOCKING_FUSE_BOX_PAKCET {		// �÷��̾ ǻ�� ���ڸ� ��������
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
	int				id;				// ���� ��ȣ�ۿ��� �������
	int				item;			// � �����۰� ��ȣ�ۿ��ϰ� �־�����, 1 : ����, 2 : ǻ��ڽ� �ڹ���
	int				index;			// �� �������� �ε���
	float			progress;		// ������� ���� ��Ȳ
};

struct SC_CHASER_RESURRECTION_PACKET {
	unsigned char	size;
	char			type;
	int				id;				// ���� ��Ȱ�ϴ���
	float			x, y, z;		// ��Ȱ ��ġ
	float			rx, ry, rz;		// ��Ȱ ����
	int				hp;				// ��Ȱ ü��
};

struct SC_RESET_FUSE_BOX_PACKET {
	unsigned char	size;
	char			type;
	int				chaserId;			// ������ id
	int				index;		// ǻ�� �ڽ��� �ε���
};

struct SC_ESCAPE_PACKET {		// ������ ��Ż�� ���� Ż���ϸ� ���� ��Ŷ
	unsigned char	size;
	char			type;
	int				id;			// ���� Ż�� �ߴ���
};

struct SC_GAME_RESULT_PACKET {
	unsigned char	size;
	char			type;
	int				score;
};

#pragma pack (pop)