#pragma once

#define BUF_SIZE 1024
#define PORT_NUM 8080
#define MAX_USER 10000

#define ID_LEN 10
#define PWD_LEN 10
#define NICKNAME_LEN 10


constexpr int PROTOCOL_NAME_SIZE = 20;
constexpr int MAX_OBJECTS = 100;


constexpr char CS_LOGIN = 0;




//===============================================================================
#pragma pack (push, 1)	
struct CS_LOGIN_PACKET {			// �α���
	unsigned char	size;
	char			type;
	char			id[ID_LEN];
	char			password[PWD_LEN];
};

// ====================================== ���� -> Ŭ�� ��Ŷ ==========================================
struct SC_LOGIN_INFO_PACKET {		// �α��� ����
	unsigned char	size;
	char			type;
	char			userName[NICKNAME_LEN];
	int				money;
	int				id;
};
