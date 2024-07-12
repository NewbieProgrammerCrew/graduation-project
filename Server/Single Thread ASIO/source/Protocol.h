#pragma once

#define BUF_SIZE 1024
#define PORT_NUM 9200
#define MAX_USER 500
#define MAX_MAP_NUM 2
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
#define MAX_JELLY_NUM 55
#define INGAME_MAX_FUSE_BOX_NUM 8
#define BOMB_SPEED 50

#define CHASER_HIT_RANGE 50

enum BombType {Stun, Explosion, Blind, NoBomb};
enum SkillType {CowBoy, Dancer, Soldier, Student, Warrior, Chaser1, Chaser2};

constexpr int PROTOCOL_NAME_SIZE = 20;
constexpr int MAX_OBJECTS = 100;


constexpr char CS_CONNECT_GAME_SERVER = 101;
constexpr char CS_MAP_LOADED = 103;
constexpr char CS_MOVE = 104;
constexpr char CS_ATTACK = 105;
constexpr char CS_PICKUP_FUSE = 106;
constexpr char CS_PRESS_F = 107;
constexpr char CS_RELEASE_F = 108;
constexpr char CS_PUT_FUSE = 109;
constexpr char CS_PICKUP_BOMB = 110;
constexpr char CS_AIM_STATE = 111;
constexpr char CS_IDLE_STATE = 112;
constexpr char CS_CANNON_FIRE = 113;
constexpr char CS_USE_SKILL = 114;
constexpr char CS_ESCAPE = 115;




//===================================================================
constexpr char SC_SIGNUP = 100;
constexpr char SC_LOGIN_INFO = 101;
constexpr char SC_LOGIN_FAIL = 102;
constexpr char SC_MAP_INFO = 103;
constexpr char SC_ADD_PLAYER = 104;
constexpr char SC_MOVE_PLAYER = 105;
constexpr char SC_ATTACK_PLAYER = 106;
constexpr char SC_HITTED = 107;
constexpr char SC_DEAD = 108;
constexpr char SC_PICKUP_FUSE =109;
constexpr char SC_NOT_INTERACTIVE = 110;
constexpr char SC_ITEM_BOX_OPENED = 111;
constexpr char SC_OPENING_ITEM_BOX = 112;
constexpr char SC_STOP_OPENING = 113;
constexpr char SC_OPENING_FUSE_BOX = 114;
constexpr char SC_FUSE_BOX_OPENED = 115;
constexpr char SC_FUSE_BOX_ACTIVE = 116;
constexpr char SC_HALF_PORTAL_GAUGE = 117;
constexpr char SC_MAX_PORTAL_GAUGE = 118;
constexpr char SC_PICKUP_BOMB = 119;
constexpr char SC_AIM_STATE = 120;
constexpr char SC_IDLE_STATE = 121;
constexpr char SC_CANNON_FIRE = 122;
constexpr char SC_BOMB_EXPLOSION = 123;
constexpr char SC_REMOVE_JELLY = 124;
constexpr char SC_USE_SKILL = 125;
constexpr char SC_CHASER_RESURRECTION = 126;
constexpr char SC_ESCAPE = 127;
constexpr char SC_REMOVE_PLAYER = 128;
constexpr char SC_SKILL_CHOOSED = 129;










//===============================================================================
#pragma pack (push, 1)	

struct CS_CONNECT_GAME_SERVER_PACKET {
	unsigned char	size;
	char			type;
	char			role[ROLE_LEN];
	int				charactorNum;		// 1~5 ������, 6~7 ���θ�
	int				GroupNum;
};

struct CS_MAP_LOADED_PACKET {		// Ŭ���̾�Ʈ map �ε� �Ϸ�
	unsigned char size;
	char type;
};

struct CS_MOVE_PACKET {				// �÷��̾� ������
	unsigned char	size;
	char			type;
	double			rx, ry, rz;
	double			x, y, z;
	double			pitch;
	double			speed;
	bool			jump;
};

struct CS_ATTACK_PACKET {			// �÷��̾� ���� �ִϸ��̼�
	unsigned char	size;
	char			type;
	double			rx, ry, rz;
	double			x, y, z;
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

struct CS_PUT_FUSE_PACKET {		
	unsigned char	size;
	char			type;
	int				fuseBoxIndex;
};

struct CS_PICKUP_BOMB_PACKET {		
	unsigned char	size;
	char			type;
	BombType		bombType;		
	int				itemBoxIndex;		
};

struct CS_AIM_STATE_PACKET {		
	unsigned char	size;
	char			type;
};

struct CS_IDLE_STATE_PACKET {		
	unsigned char	size;
	char			type;
};

struct CS_CANNON_FIRE_PACKET {
	unsigned char	size;
	char			type;
	double			x, y, z;
	double			rx, ry, rz;
};

struct CS_USE_SKILL_PACKET {
	unsigned char	size;
	char			type;
	SkillType		skill_type;
};


struct CS_ESCAPE_PACKET {
	unsigned char	size;
	char			type;
};



// ====================================== ���� -> Ŭ�� ��Ŷ ==========================================

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
	double			x, y, z;
	char			role[PROTOCOL_NAME_SIZE];
	int				charactorNum;				// 1~5 ������, 6~7 ���θ�
};

struct SC_MOVE_PLAYER_PACKET {		// �÷��̾� ������
	unsigned char	size;
	char			type;
	int				id;
	double			x, y, z;
	double			rx, ry, rz;
	double			pitch;
	double			speed;
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
	BombType		bomb_type;
};

struct SC_OPENING_ITEM_BOX_PACKET {
	unsigned char	size;
	char			type;
	int				id;
	int				index;
	double			progress;
};

struct SC_STOP_OPENING_PACKET {
	unsigned char	size;
	char			type;
	int				id;				
	int				item;			
	int				index;			
	double			progress;		
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

struct SC_PICKUP_BOMB_PACKET {			
	unsigned char	size;
	char			type;
	int				id;			
	int				bombType;	
	int				itemBoxIndex;	
	int				leftBombType;	
	int				bombIndex;
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

struct SC_CANNON_FIRE_PACKET{
	unsigned char	size;
	char			type;
	int				id;
	int				bomb_index;
	double			x, y, z;
	double			rx, ry, rz;
	BombType		bomb_type;
};

struct SC_BOMB_EXPLOSION_PACKET {
	unsigned char	size;
	char			type;
	int				bomb_index;
};

struct SC_REMOVE_JELLY_PACKET {
	unsigned char	size;
	char			type;
	int				jellyIndex;
	float			b_x, b_y, b_z;
};

struct SC_USE_SKILL_PACKET {
	unsigned char	size;
	char			type;
	SkillType		skill_type;
	int				id;
};

struct SC_CHASER_RESURRECTION_PACKET {
	unsigned char	size;
	char			type;
	int				id;
	double			x, y, z;
	double			rx, ry, rz;
	int				hp;
};

struct SC_ESCAPE_PACKET {
	unsigned char	size;
	char			type;
	int				id;
	bool			die;
	int				score;
	bool			win;
};
struct SC_SKILL_CHOOSED_PACKET {
	unsigned char	size;
	char			type;
	SkillType		skill_type;
};




#pragma pack (pop)