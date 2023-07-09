#include <iostream>
#include <array>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include "protocol.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

enum COMP_TYPE { OP_ACCEPT, OP_RECV, OP_SEND };
class OVER_EXP {
public:
    WSAOVERLAPPED over;
    WSABUF wsabuf;
    char send_buf[MAX_BUFFER_SIZE];
    COMP_TYPE comp_type;
    OVER_EXP()
    {
        wsabuf.len = MAX_BUFFER_SIZE;
        wsabuf.buf = send_buf;
        comp_type = OP_RECV;
        ZeroMemory(&over, sizeof(over));
    }
    OVER_EXP(char* packet)
    {
        wsabuf.len = packet[0];
        wsabuf.buf = send_buf;
        ZeroMemory(&over, sizeof(over));
        comp_type = OP_SEND;
        memcpy(send_buf, packet, packet[0]);
    }
};

class SESSION {
    OVER_EXP recv;
public:
    bool in_use;
    SOCKET socket;
    int id;
    float x, y, z;
    int	prev_remain;

    SESSION() : socket(0), in_use(false)
    {
        id = -1;
        x = y = z = 0;
        prev_remain = 0;
    }
    
    ~SESSION() {}

    void do_recv()
    {
        DWORD recv_flag = 0;
        memset(&recv.over, 0, sizeof(recv.over));
        recv.wsabuf.len = MAX_BUFFER_SIZE - prev_remain;
        recv.wsabuf.buf = recv.send_buf + prev_remain;
        WSARecv(socket, &recv.wsabuf, 1, 0, &recv_flag, &recv.over, 0);
    }

    void do_send(void* packet)
    {
        OVER_EXP* sdata = new OVER_EXP{ reinterpret_cast<char*>(packet) };
        WSASend(socket, &sdata->wsabuf, 1, 0, 0, &sdata->over, 0);
    }
    void send_login_info_packet()
    {
        SC_LOGIN_INFO_PACKET p;
        p.id = id;
        p.size = sizeof(SC_LOGIN_INFO_PACKET);
        p.type = SC_LOGIN_INFO;
        p.x = x;
        p.y = y;
        p.z = z;
        do_send(&p);
    }
    void send_move_packet(int c_id);
};

array<SESSION, MAX_CLIENTS> clients;

int main() {

    HANDLE h_iocp;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }

    // 소켓 생성 및 바인딩
    SOCKET listenSocket = WSASocket(AF_INET6, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in6 serverAddr{};
    serverAddr.sin6_family = AF_INET6;
    serverAddr.sin6_addr = in6addr_any;
    serverAddr.sin6_port = htons(12345);

    if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind socket: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // IOCP 생성 및 완료 포트에 소켓 연결
    completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
    if (completionPort == nullptr) {
        std::cerr << "Failed to create IOCP: " << GetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSocket), completionPort, 0, 0) == nullptr) {
        std::cerr << "Failed to associate socket with IOCP: " << GetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // 작업 스레드 생성
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    for (DWORD i = 0; i < sysInfo.dwNumberOfProcessors; ++i) {
        HANDLE threadHandle = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
        if (threadHandle == nullptr) {
            std::cerr << "Failed to create worker thread: " << GetLastError() << std::endl;
            closesocket(listenSocket);
            CloseHandle(completionPort);
            WSACleanup();
            return 1;
        }

        CloseHandle(threadHandle);
    }

    // 클라이언트 연결 대기
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        CloseHandle(completionPort);
        WSACleanup();
        return 1;
    }

    std::cout << "Server started" << std::endl;

    while (true) {
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
            continue;
        }

        // 클라이언트 컨텍스트 생성 및 IOCP에 소켓 연결
        ClientContext* clientContext = new ClientContext;
        ZeroMemory(&clientContext->overlapped, sizeof(OVERLAPPED));
        clientContext->socket = clientSocket;
        clientContext->dataBuffer.buf = clientContext->buffer;
        clientContext->dataBuffer.len = MAX_BUFFER_SIZE;

        if (CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), completionPort, reinterpret_cast<ULONG_PTR>(clientContext), 0) == nullptr) {
            std::cerr << "Failed to associate client socket with IOCP: " << GetLastError() << std::endl;
            closesocket(clientSocket);
            delete clientContext;
            continue;
        }

        // 데이터 수신 대기
        DWORD flags = 0;
        if (WSARecv(clientSocket, &clientContext->dataBuffer, 1, nullptr, &flags, &clientContext->overlapped, nullptr) == SOCKET_ERROR) {
            if (WSAGetLastError() != WSA_IO_PENDING) {
                std::cerr << "WSARecv failed: " << WSAGetLastError() << std::endl;
                closesocket(clientSocket);
                delete clientContext;
                continue;
            }
        }

        std::cout << "Client connected" << std::endl;
    }

    closesocket(listenSocket);
    CloseHandle(completionPort);
    WSACleanup();

    return 0;
}
