#pragma once
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

constexpr char SC_LOGIN_INFO = 2;
constexpr char SC_ADD_PLAYER = 3;
constexpr char SC_REMOVE_PLAYER = 4;
constexpr char SC_MOVE_PLAYER = 5;
constexpr char SC_CHANGE_HP = 6;
constexpr char SC_ATTACK_PLAYER = 7;
constexpr char SC_HITTED = 8;
constexpr char SC_DEAD = 9;

#pragma pack (push, 1)
struct CS_LOGIN_PACKET {
	unsigned char	size;
	char			type;
	char			role[PROTOCOL_NAME_SIZE];
};

struct CS_MOVE_PACKET {
	unsigned char	size;
	char			type;
	float			rx, ry, rz;
	float			x, y, z;
	float			speed;
};

struct CS_ATTACK_PACKET {
	unsigned char	size;
	char			type;
	int				id;
	float			ry;
	float			x, y, z;
};

struct CS_HITTED_PACKET {
    unsigned char size;
    char type;
    int id;
    float hp;
};

struct SC_LOGIN_INFO_PACKET {
	unsigned char	size;
	char			type;
	int				id;
	float			x, y, z;
};

struct SC_ADD_PLAYER_PACKET {
	unsigned char	size;
	char			type;
	int				id;
	int				_hp;
	float			x, y, z;
	char			role[PROTOCOL_NAME_SIZE];
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
	float			x, y, z;
	float			rx, ry, rz;
	float			speed;
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
    unsigned char size;
    char type;
    int id;
    float x, y, z;
    //float  ry;
};


#pragma pack (pop)