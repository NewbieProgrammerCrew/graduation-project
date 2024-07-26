#pragma once

#include<iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include "../../../../../Server/Single Thread ASIO/source/Protocol.h" 
#include "../../../../../Server/Lobby Server/protocol.h" 
#include "HAL/Runnable.h"
#include "CoreMinimal.h"

#pragma comment (lib,"MSWSock.lib")
#pragma comment (lib, "WS2_32.LIB")

void CALLBACK send_l_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag);
void CALLBACK send_g_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag);
void CALLBACK recv_l_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag);
void CALLBACK recv_g_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag);


class NPC_WORLD_API NetworkingThread 
{
public:
    NetworkingThread();
    ~NetworkingThread();

};

class WSA_OVER_EX
{
public:
    WSAOVERLAPPED _wsaover;
    WSABUF _wsabuf;
    unsigned char _buf[BUF_SIZE];

public:
    WSA_OVER_EX();
    WSA_OVER_EX(char byte, void* buf);
    WSAOVERLAPPED& getWsaOver() { return _wsaover; };
};

class FSocketThread : public FRunnable
{
public:
    FSocketThread();
    FSocketThread(UGameInstance* inGameInstance);
    virtual void Stop() override;
    virtual void Exit() override;
    virtual uint32_t Run() override;
    void Destroy();
    class AMain* _MainClass = nullptr;
    class AJellyManager* _JellyManager = nullptr;
    class ACh_PlayerController* _MyController = nullptr;
    class APlayerManager* _PlayerManager = nullptr;
    class AFuseManager* _FuseManager = nullptr;
    class AFuseBoxManager* _FuseBoxManager = nullptr;
    class APortalManager* _PortalManager = nullptr;
    class AItemBoxManager* _ItemBoxManager = nullptr;
    class ABombManager* _BombManager = nullptr;
    class UMyGameInstance* _gameInstance = nullptr;

    bool IsRunning = false;
    bool IsInGame = false;
    bool IsConnected = false;
    double CycleTime{};
    WSA_OVER_EX l_recv_over_ex;
    WSA_OVER_EX g_recv_over_ex;
    SOCKET gs_socket;
    SOCKET ls_socket;

    int _prev_size = 0;

    int my_lobby_id;
    int my_game_id;
    void g_processpacket(unsigned char* buf);
    void l_processpacket(unsigned char* buf);
    void error_display(const char* msg, int err_no);
    void InitializeManagers();
    char IPAddress[20];
    int32 BytesReceived;
    int32 BytesSent;
};
