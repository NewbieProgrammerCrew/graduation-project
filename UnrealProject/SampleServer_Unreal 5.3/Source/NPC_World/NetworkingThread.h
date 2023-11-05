#pragma once

#include<iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include "../../../../Server/ServerTest/ServerTest/protocol.h"
#include "HAL/Runnable.h"
#include "CoreMinimal.h"

#pragma comment (lib,"MSWSock.lib")
#pragma comment (lib, "WS2_32.LIB")

void CALLBACK send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag);
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag);

enum IOCPOP
{
	OP_RECV,
	OP_SEND,
	OP_ACCEPT
};

class NPC_WORLD_API NetworkingThread {
 public:
  NetworkingThread();
  ~NetworkingThread();

};

class WSA_OVER_EX {
       public:
        WSAOVERLAPPED _wsaover;
        IOCPOP _iocpop;
        WSABUF _wsabuf;
        unsigned char _buf[BUF_SIZE];

       public:
        WSA_OVER_EX();
        WSA_OVER_EX(IOCPOP iocpop, char byte, void* buf);
        WSAOVERLAPPED& getWsaOver() { return _wsaover; };
};

class FSocketThread : public FRunnable {
       public:
        FSocketThread();

        void Stop();
        virtual uint32_t Run() override;

        class AMain* _MainClass = nullptr;
        class AMyPlayerController* _MyController = nullptr;
        class APlayerManager* _PlayerManager = nullptr;

        bool IsRunning = false;
        bool IsConnected = false;
        double CycleTime{};
        WSA_OVER_EX _recv_over_ex;
        SOCKET s_socket;

        int _prev_size = 0;

        int my_id;
        void processpacket(unsigned char* buf);
        void error_display(const char* msg, int err_no);

        char IPAddress[20];
        int32 BytesReceived;
        int32 BytesSent;
};
