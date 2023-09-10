#pragma once
constexpr int PORT_NUM = 8080;
constexpr int BUF_SIZE = 512;
constexpr int PROTOCOL_NAME_SIZE = 20;

constexpr int W_WIDTH = 8;
constexpr int W_HEIGHT = 8;

// Packet ID
constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;

constexpr char SC_LOGIN_INFO = 2;
constexpr char SC_ADD_PLAYER = 3;
constexpr char SC_REMOVE_PLAYER = 4;
constexpr char SC_MOVE_PLAYER = 5;

#pragma pack (push, 1)
struct CS_LOGIN_PACKET {
    unsigned char size;
    char    type;
    char    name[PROTOCOL_NAME_SIZE];
};

struct CS_MOVE_PACKET {
    unsigned char size;
    char    type;
    float   rx, ry, rz;
    short   direction;  // 0 : Fwd , 1 : back, 2 : LEFT, 3 : RIGHT 4: jump 5
    float   speed;
};

struct SC_LOGIN_INFO_PACKET {
    unsigned char size;
    char        type;
    int         id;
    float    x, y, z;
};

struct SC_ADD_PLAYER_PACKET {
    unsigned char size;
    char    type;
    int    id;
    float    x, y, z;
    char    name[PROTOCOL_NAME_SIZE];
};

struct SC_REMOVE_PLAYER_PACKET {
    unsigned char size;
    char    type;
    int    id;
};

struct SC_MOVE_PLAYER_PACKET {
    unsigned char size;
    char    type;
    int    id;
    float    x, y, z;
    float   rx, ry, rz;
    float speed;
};

#pragma pack (pop)