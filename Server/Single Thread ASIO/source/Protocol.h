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
//===================================================================
constexpr char SC_SIGNUP = 0;
constexpr char SC_LOGIN_INFO = 1;
constexpr char SC_LOGIN_FAIL = 2;
constexpr char SC_MAP_INFO = 3;
constexpr char SC_ADD_PLAYER = 4;





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

struct SC_REMOVE_PLAYER_PACKET {	// �÷��̾� ����
	unsigned char	size;
	char			type;
	int				id;
};
#pragma pack (pop)