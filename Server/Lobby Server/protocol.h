#pragma once

#define BUF_SIZE 1024
#define PORT_NUM 7777
#define MAX_USER 500
#define MAX_MAP_NUM 3
#define ROLE_LEN 8

#define ID_LEN 10
#define PWD_LEN 10
#define NICKNAME_LEN 10
#define ADDRESS_LEN 20

constexpr unsigned char CS_SIGNUP = 0;
constexpr unsigned char CS_LOGIN = 1;
constexpr unsigned char CS_ROLE = 2;


//===================================================================
constexpr unsigned char SC_SIGNUP = 0;
constexpr unsigned char SC_LOGIN_INFO = 1;
constexpr unsigned char SC_LOGIN_FAIL = 2;
constexpr unsigned char SC_GAME_START = 3;


//===============================================================================
#pragma pack (push, 1)	

struct CS_SIGNUP_PACKET {			// ȸ������
	unsigned char	size;
	unsigned char			type;
	char			id[ID_LEN];
	char			password[PWD_LEN];
	char			userName[NICKNAME_LEN];
};

struct CS_LOGIN_PACKET {			// �α���
	unsigned char	size;
	unsigned char			type;
	char			id[ID_LEN];
	char			password[PWD_LEN];
};

struct CS_ROLE_PACKET {			// ���� ����
	unsigned char	size;
	unsigned char			type;
	char			role[ROLE_LEN];
	int				charactorNum;		// 1~5 ������, 6~7 ���θ�
};


// ====================================== ���� -> Ŭ�� ��Ŷ ==========================================

struct SC_SIGNUP_PACKET {			// ȭ������ ���� Ȥ�� ����
	unsigned char	size;
	unsigned char			type;
	bool			success;
	int				errorCode;
	int				id;
};

struct SC_LOGIN_INFO_PACKET {		// �α��� ����
	unsigned char	size;
	unsigned char			type;
	char			userName[NICKNAME_LEN];
	int				money;
	int				id;
};

struct SC_LOGIN_FAIL_PACKET {		// �α��� ����
	unsigned char	size;
	unsigned char			type;
	int				errorCode;
	int				id;
};

struct SC_GAME_START_PACKET {
	unsigned char	size;
	unsigned char			type;
	char			address[ADDRESS_LEN];
	int				portNum;
};
#pragma pack (pop)