#include <iostream>
#include <array>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include "protocol.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "MSWSock.lib")


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
    char name[NAME_SIZE];
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

int get_new_client_id()
{
    for (int i = 0; i < MAX_CLIENTS; ++i)
        if (clients[i].in_use == false)
            return i;
    return -1;
}


void disconnect(int c_id)
{
    for (auto& pl : clients) {
        if (pl.in_use == false) continue;
        if (pl.id == c_id) continue;
        SC_REMOVE_PLAYER_PACKET p;
        p.id = c_id;
        p.size = sizeof(p);
        p.type = SC_REMOVE_PLAYER;
        pl.do_send(&p);
    }
    closesocket(clients[c_id].socket);
    clients[c_id].in_use = false;
}

void process_packet(int c_id, char* packet)
{
    switch (packet[1]) {
    case CS_LOGIN: {
        CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
        strcpy_s(clients[c_id].name, p->name);
        clients[c_id].send_login_info_packet();

        for (auto& pl : clients) {
            if (false == pl.in_use) continue;
            if (pl.id == c_id) continue;
            SC_ADD_PLAYER_PACKET add_packet;
            add_packet.id = c_id;
            strcpy_s(add_packet.name, p->name);
            add_packet.size = sizeof(add_packet);
            add_packet.type = SC_ADD_PLAYER;
            add_packet.x = clients[c_id].x;
            add_packet.y = clients[c_id].y;
            pl.do_send(&add_packet);
        }
        for (auto& pl : clients) {
            if (false == pl.in_use) continue;
            if (pl.id == c_id) continue;
            SC_ADD_PLAYER_PACKET add_packet;
            add_packet.id = pl.id;
            strcpy_s(add_packet.name, pl.name);
            add_packet.size = sizeof(add_packet);
            add_packet.type = SC_ADD_PLAYER;
            add_packet.x = pl.x;
            add_packet.y = pl.y;
            clients[c_id].do_send(&add_packet);
        }
        break;
    }

    }
}

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

    sockaddr_in6 serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin6_family = AF_INET6;
    serverAddr.sin6_addr = in6addr_any;
    serverAddr.sin6_port = htons(PORT_NUM);

    if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind socket: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    listen(listenSocket, SOMAXCONN);
    sockaddr_in6 cl_addr;
    int addr_size = sizeof(cl_addr);
    int client_id = 0;

    // IOCP 생성 및 완료 포트에 소켓 연결
    h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
    if (h_iocp == nullptr) {
        std::cerr << "Failed to create IOCP: " << GetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSocket), h_iocp, 9999, 0) == nullptr) {
        std::cerr << "Failed to associate socket with IOCP: " << GetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    SOCKET c_socket = WSASocket(AF_INET6, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    OVER_EXP a_over;
    a_over.comp_type = OP_ACCEPT;
    AcceptEx(listenSocket, c_socket, a_over.send_buf, 0, addr_size + 16, addr_size + 16, 0, &a_over.over);

    while (true) {
        DWORD num_bytes;
        ULONG_PTR key;
        WSAOVERLAPPED* over = nullptr;
        BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &over, INFINITE);
        OVER_EXP* ex_over = reinterpret_cast<OVER_EXP*>(over);
        if (FALSE == ret) {
            if (ex_over->comp_type == OP_ACCEPT) cout << "Accept Error";
            else {
                cout << "GQCS Error on client[" << key << "]\n";
                disconnect(static_cast<int>(key));
                if (ex_over->comp_type == OP_SEND) delete ex_over;
                continue;
            }
        }

        switch (ex_over->comp_type) {
        case OP_ACCEPT: {
            int client_id = get_new_client_id();
            if (client_id != -1) {
                clients[client_id].in_use = true;
                clients[client_id].x = 0;
                clients[client_id].y = 0;
                clients[client_id].z = 0;
                clients[client_id].id = client_id;
                clients[client_id].name[0] = 0;
                clients[client_id].prev_remain = 0;
                clients[client_id].socket = c_socket;
                CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket), h_iocp, client_id, 0);
                clients[client_id].do_recv();
                c_socket = WSASocket(AF_INET6, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
            }
            else {
                cout << "Max user exceeded.\n";
            }
            ZeroMemory(&a_over.over, sizeof(a_over.over));
            AcceptEx(listenSocket, c_socket, a_over.send_buf, 0, addr_size + 16, addr_size + 16, 0, &a_over.over);
            break;
        }
        case OP_RECV: {
            int remain_data = num_bytes + clients[key].prev_remain;
            char* p = ex_over->send_buf;
            while (remain_data > 0) {
                int packet_size = p[0];
                if (packet_size <= remain_data) {
                    process_packet(static_cast<int>(key), p);
                    p = p + packet_size;
                    remain_data = remain_data - packet_size;
                }
                else break;
            }
            clients[key].prev_remain = remain_data;
            if (remain_data > 0) {
                memcpy(ex_over->send_buf, p, remain_data);
            }
            clients[key].do_recv();
            break;
        }
        case OP_SEND:
            delete ex_over;
            break;
        }
    }
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
