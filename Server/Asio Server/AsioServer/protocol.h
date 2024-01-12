#pragma once

#define BUF_SIZE 1024
#define PORT_NUM 8080
#define MAX_USER 3

#define ID_LEN 10
#define PWD_LEN 10
#define NICKNAME_LEN 10


constexpr int PROTOCOL_NAME_SIZE = 20;
constexpr int MAX_OBJECTS = 100;


constexpr char CS_SIGNUP = 0;
constexpr char CS_LOGIN = 1;
//===================================================================
constexpr char SC_SIGNUP = 0;
constexpr char SC_LOGIN_INFO = 1;
constexpr char SC_LOGIN_FAIL = 2;





//===============================================================================
#pragma pack (push, 1)	

struct CS_SIGNUP_PACKET {			// 회원가입
	unsigned char	size;
	char			type;
	char			id[ID_LEN];
	char			password[PWD_LEN];
	char			userName[NICKNAME_LEN];
};

struct CS_LOGIN_PACKET {			// 로그인
	unsigned char	size;
	char			type;
	char			id[ID_LEN];
	char			password[PWD_LEN];
};


// ====================================== 서버 -> 클라 패킷 ==========================================

struct SC_SIGNUP_PACKET {			// 화원가입 실패 혹은 성공
	unsigned char	size;
	char			type;
	bool			success;
	int				errorCode;
	int				id;
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


