#pragma once

#define BUF_SIZE 1024
#define PORT_NUM 9200
#define MAX_USER 500
#define MAX_MAP_NUM 4
#define ROLE_LEN 8


#define COL_SECTOR_SIZE 800
#define MAP_X 25200
#define MAP_Y 25200
#define MAP_Z 7300
#define SEC_TO_MICRO 1000000

#define ID_LEN 10
#define PWD_LEN 10
#define NICKNAME_LEN 10
#define MAX_FUSE_BOX_NUM 16
#define MAX_JELLY_NUM 70
#define INGAME_MAX_FUSE_BOX_NUM 8
#define BOMB_SPEED 50

#define CHASER_HIT_RANGE 50
#define TOTAL_NUMBER_OF_MAPS 4

enum BombType {Stun, Explosion, Blind, NoBomb};
enum SkillType {CowBoy, Dancer, Soldier, Student, Warrior, Chaser1, Chaser2};
enum STATUS {Disconnected, Dead, Escaped};

constexpr int PROTOCOL_NAME_SIZE = 20;
constexpr int MAX_OBJECTS = 100;



constexpr unsigned char CS_CONNECT_GAME_SERVER = 101;
constexpr unsigned char CS_MAP_LOADED = 103;
constexpr unsigned char CS_MOVE = 104;
constexpr unsigned char CS_ATTACK = 105;
constexpr unsigned char CS_PICKUP_FUSE = 106;
constexpr unsigned char CS_PRESS_F = 107;
constexpr unsigned char CS_RELEASE_F = 108;
constexpr unsigned char CS_PUT_FUSE = 109;
constexpr unsigned char CS_PICKUP_BOMB = 110;
constexpr unsigned char CS_AIM_STATE = 111;
constexpr unsigned char CS_IDLE_STATE = 112;
constexpr unsigned char CS_CANNON_FIRE = 113;
constexpr unsigned char CS_USE_SKILL = 114;
constexpr unsigned char CS_ESCAPE = 115;
constexpr unsigned char CS_GOTO_LOBBY = 116;
constexpr unsigned char CS_PICK_UP_INK = 117;
constexpr unsigned char CS_PICK_UP_STUN = 118;
constexpr unsigned char CS_PICK_UP_EXPLOSION = 119;
constexpr unsigned char CS_PORTAL_GAUGE_HALF = 120;




//===================================================================
constexpr unsigned char SC_MAP_INFO = 103;
constexpr unsigned char SC_ADD_PLAYER = 104;
constexpr unsigned char SC_MOVE_PLAYER = 105;
constexpr unsigned char SC_ATTACK_PLAYER = 106;
constexpr unsigned char SC_HITTED = 107;
constexpr unsigned char SC_DEAD = 108;
constexpr unsigned char SC_PICKUP_FUSE =109;
constexpr unsigned char SC_NOT_INTERACTIVE = 110;
constexpr unsigned char SC_ITEM_BOX_OPENED = 111;
constexpr unsigned char SC_OPENING_ITEM_BOX = 112;
constexpr unsigned char SC_STOP_OPENING = 113;
constexpr unsigned char SC_OPENING_FUSE_BOX = 114;
constexpr unsigned char SC_FUSE_BOX_OPENED = 115;
constexpr unsigned char SC_FUSE_BOX_ACTIVE = 116;
constexpr unsigned char SC_HALF_PORTAL_GAUGE = 117;
constexpr unsigned char SC_MAX_PORTAL_GAUGE = 118;
constexpr unsigned char SC_PICKUP_BOMB = 119;
constexpr unsigned char SC_AIM_STATE = 120;
constexpr unsigned char SC_IDLE_STATE = 121;
constexpr unsigned char SC_CANNON_FIRE = 122;
constexpr unsigned char SC_BOMB_EXPLOSION = 123;
constexpr unsigned char SC_REMOVE_JELLY = 124;
constexpr unsigned char SC_USE_SKILL = 125;
constexpr unsigned char SC_CHASER_RESURRECTION = 126;
constexpr unsigned char SC_ESCAPE = 127;
constexpr unsigned char SC_REMOVE_PLAYER = 128;
constexpr unsigned char SC_SKILL_CHOOSED = 129;
constexpr unsigned char SC_CHASER_WIN = 130;
constexpr unsigned char SC_PICK_UP_INK = 131;
constexpr unsigned char SC_PICK_UP_STUN = 132;
constexpr unsigned char SC_PICK_UP_EXPLOSION = 133;










//===============================================================================
#pragma pack (push, 1)	

struct CS_CONNECT_GAME_SERVER_PACKET {
	unsigned char	size;
	unsigned char			type;
	char			role[ROLE_LEN];
	int				charactorNum;		// 1~5 생존자, 6~7 살인마
	int				GroupNum;
};

struct CS_MAP_LOADED_PACKET {		// 클라이언트 map 로드 완료
	unsigned char size;
	unsigned char type;
};

struct CS_MOVE_PACKET {				// 플레이어 움직임
	unsigned char	size;
	unsigned char			type;
	float			rx, ry, rz;
	float			x, y, z;
	float			pitch;
	float			speed;
	bool			jump;
};

struct CS_ATTACK_PACKET {			// 플레이어 때림 애니메이션
	unsigned char	size;
	unsigned char			type;
	float			rx, ry, rz;
	float			x, y, z;
};

struct CS_PICKUP_FUSE_PACKET {		// 플레이어 아이템 얻음
	unsigned char	size;
	unsigned char			type;
	int 			fuseIndex;			// 몇번째 인덱스의 퓨즈인지
};

struct CS_PRESS_F_PACKET {
	unsigned char	size;
	unsigned char	type;
	int				item;	// 1 : item box, 2 : fuse box
	int				index;
};

struct CS_RELEASE_F_PACKET {
	unsigned char	size;
	unsigned char			type;
	int				item;
	int				index;
};

struct CS_PUT_FUSE_PACKET {		
	unsigned char	size;
	unsigned char			type;
	int				fuseBoxIndex;
};

struct CS_PICKUP_BOMB_PACKET {		
	unsigned char	size;
	unsigned char	type;
	BombType		bombType;		
	int				itemBoxIndex;		
};

struct CS_AIM_STATE_PACKET {		
	unsigned char	size;
	unsigned char			type;
};

struct CS_IDLE_STATE_PACKET {		
	unsigned char	size;
	unsigned char			type;
};

struct CS_CANNON_FIRE_PACKET {
	unsigned char	size;
	unsigned char			type;
	float			x, y, z;
	float			rx, ry, rz;
};

struct CS_USE_SKILL_PACKET {
	unsigned char	size;
	unsigned char	type;
	SkillType		skill_type;
};


struct CS_ESCAPE_PACKET {
	unsigned char	size;
	unsigned char	type;
};

struct CS_GOTO_LOBBY_PACKET {
	unsigned char	size;
	unsigned char	type;
};

struct CS_PICK_UP_INK_PACKET {
	unsigned char	size;
	unsigned char	type;
};

struct CS_PICK_UP_STUN_PACKET {
	unsigned char	size;
	unsigned char	type;
};

struct CS_PICK_UP_EXPLOSION_PACKET {
	unsigned char	size;
	unsigned char	type;
};

struct CS_PORTAL_GAUGE_HALF_PACKET {
	unsigned char	size;
	unsigned char	type;
};

// ====================================== 서버 -> 클라 패킷 ==========================================

struct SC_MAP_INFO_PACKET {		// 맵 정보 전달
	unsigned char	size;
	unsigned char	type;
	int				id;
	int				mapid;
	int				patternid;
	int				fusebox[8];
	int				fusebox_color[8];
};

struct SC_ADD_PLAYER_PACKET {		// 플레이어 추가
	unsigned char	size;
	unsigned char			type;
	int				id;
	int				_hp;
	float			x, y, z;
	char			role[PROTOCOL_NAME_SIZE];
	int				charactorNum;				// 1~5 생존자, 6~7 살인마
};

struct SC_MOVE_PLAYER_PACKET {		// 플레이어 움직임
	unsigned char	size;
	unsigned char			type;
	int				id;
	float			x, y, z;
	float			rx, ry, rz;
	float			pitch;
	float			speed;
	bool			jump;
};

struct SC_ATTACK_PLAYER_PACKET {	// 플레이어 공격 모션
	unsigned char	size;
	unsigned char			type;
	int				id;
};

struct SC_HITTED_PACKET {			// 플레이어 맞음
	unsigned char   size;
	unsigned char   type;
	int             id;
	BombType		bombType;
	int             _hp;
};

struct SC_DEAD_PACKET {				// 플레이어 죽음
	unsigned char   size;
	unsigned char            type;
	int             id;
	int             _hp;
};

struct SC_PICKUP_FUSE_PACKET {			// 플레이가 퓨즈를 얻음
	unsigned char	size;
	unsigned char			type;
	int				id;			// 퓨즈를 얻은 플레이어 아이디
	int				index;		// 얻은 퓨즈의 인덱스
};

struct SC_REMOVE_PLAYER_PACKET {	// 플레이어 삭제
	unsigned char	size;
	unsigned char	type;
	int				id;
	STATUS			status;
};

struct SC_NOT_INTERACTIVE_PACKET {
	unsigned char	size;
	unsigned char	type;
};

struct SC_ITEM_BOX_OPENED_PACKET {
	unsigned char	size;
	unsigned char			type;
	int				index;
	BombType		bomb_type;
};

struct SC_OPENING_ITEM_BOX_PACKET {
	unsigned char	size;
	unsigned char			type;
	int				id;
	int				index;
	float			progress;
};

struct SC_STOP_OPENING_PACKET {
	unsigned char	size;
	unsigned char			type;
	int				id;				
	int				item;			
	int				index;			
	float			progress;		
};

struct SC_OPENING_FUSE_BOX_PACKET {
	unsigned char	size;
	unsigned char			type;
	int				id;
	int				index;
	float			progress;
};


struct SC_FUSE_BOX_OPENED_PACKET {
	unsigned char	size;
	unsigned char			type;
	int				index;
};

struct SC_FUSE_BOX_ACTIVE_PACKET {
	unsigned char	size;
	unsigned char			type;
	int				fuseBoxIndex;
};
struct SC_HALF_PORTAL_GAUGE_PACKET {
	unsigned char	size;
	unsigned char			type;
};
struct SC_MAX_PORTAL_GAUGE_PACKET {
	unsigned char	size;
	unsigned char			type;
};

struct SC_PICKUP_BOMB_PACKET {			
	unsigned char	size;
	unsigned char			type;
	int				id;			
	int				bombType;	
	int				itemBoxIndex;	
	int				leftBombType;	
	int				bombIndex;
};

struct SC_AIM_STATE_PACKET {		
	unsigned char	size;
	unsigned char			type;
	int				id;
};

struct SC_IDLE_STATE_PACKET {		
	unsigned char	size;
	unsigned char			type;
	int				id;
};

struct SC_CANNON_FIRE_PACKET{
	unsigned char	size;
	unsigned char			type;
	int				id;
	int				bomb_index;
	float			x, y, z;
	float			rx, ry, rz;
	BombType		bomb_type;
};

struct SC_BOMB_EXPLOSION_PACKET {
	unsigned char	size;
	unsigned char			type;
	int				bomb_index;
};

struct SC_REMOVE_JELLY_PACKET {
	unsigned char	size;
	unsigned char			type;
	int				jellyIndex;
	float			b_x, b_y, b_z;
};

struct SC_USE_SKILL_PACKET {
	unsigned char	size;
	unsigned char			type;
	SkillType		skill_type;
	int				id;
};

struct SC_CHASER_RESURRECTION_PACKET {
	unsigned char	size;
	unsigned char			type;
	int				id;
	float			x, y, z;
	float			rx, ry, rz;
	int				hp;
};

struct SC_ESCAPE_PACKET {
	unsigned char	size;
	unsigned char	type;
	int				id;
	bool			die;
	int				score;
	bool			runner_win;
};

struct SC_SKILL_CHOOSED_PACKET {
	unsigned char	size;
	unsigned char	type;
	SkillType		skill_type;
};

struct SC_CHASER_WIN_PACKET {
	unsigned char	size;
	unsigned char	type;
};

struct SC_PICK_UP_INK_PACKET {
	unsigned char	size;
	unsigned char	type;
	int				c_id;
};

struct SC_PICK_UP_STUN_PACKET {
	unsigned char	size;
	unsigned char	type;
	int				c_id;
};

struct SC_PICK_UP_EXPLOSION_PACKET {
	unsigned char	size;
	unsigned char	type;
	int				c_id;
};


#pragma pack (pop)