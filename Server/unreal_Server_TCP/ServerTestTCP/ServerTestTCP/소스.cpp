#include <iostream>
#include <WinSock2.h>
#include <MSWSock.h>
#include <ws2tcpip.h>
#include <thread>
#include <vector>
#pragma comment(lib, "ws2_32.lib")

const int BUFFER_SIZE = 512;

class Player {
public:
    SOCKET MySocket;
    int id;
    std::string Pos;
    Player(SOCKET newSocket, int newid) : MySocket(newSocket), id(newid) {
        Pos = "1429,1109,92";
    }
};

std::vector<Player> Players;
int currentPlayerID = 0;

std::string ReceiveData(SOCKET ClientSocket) {
    char buffer[BUFFER_SIZE] = { 0 };
    int bytesReceived = recv(ClientSocket, buffer, BUFFER_SIZE, 0);
    if (bytesReceived > 0) {
        return std::string(buffer, bytesReceived);
    }
    return "";
}

void SendData(SOCKET ClientSocket, const std::string& Data) {
    send(ClientSocket, Data.c_str(), Data.size(), 0);
}

void HandleClient(SOCKET ClientSocket) {
    std::cout << "Client connected!\n";

    Player newPlayer(ClientSocket, currentPlayerID++);
    Players.push_back(newPlayer);

    std::cout << "Assigned ID: " << newPlayer.id << " to the new client.\n";

    while (true) {
        std::string receivedData = ReceiveData(ClientSocket);
        if (receivedData.empty()) {
            break;
        }
        std::cout << "Received from client (ID: " << newPlayer.id << "): " << receivedData << "\n";

        // Ŭ���̾�Ʈ�κ��� ���� ��ǥ�� ����
       // newPlayer.Pos = receivedData;
        Players[newPlayer.id].Pos = receivedData;
        // �� ���������� �ܼ��� �ٸ� Ŭ���̾�Ʈ�� ��ǥ�� ��ȯ�մϴ�.
        // �����δ� �ʿ��� ������ ���� �ٸ� ó���� �� �� �ֽ��ϴ�.
        std::string responseData;
        for (const auto& player : Players) {
            if (player.id != newPlayer.id) {
                responseData = player.Pos;
                break;
            }
        }

        SendData(ClientSocket, responseData);
    }

    std::cout << "Client (ID: " << newPlayer.id << ") disconnected.\n";
    closesocket(ClientSocket);



}
int main() {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed: " << iResult << "\n";
        return 1;
    }

    SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        std::cout << "Socket failed: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in service;
    service.sin_family = AF_INET;
    ::inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
    service.sin_port = htons(8080);

    if (bind(ListenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        std::cout << "Bind failed: " << WSAGetLastError() << "\n";
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "Listen failed: " << WSAGetLastError() << "\n";
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Waiting for client connections...\n";
    while (true) {
        SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            std::cout << "Accept failed: " << WSAGetLastError() << "\n";
            continue;
        }

        std::thread clientThread(HandleClient, ClientSocket);
        clientThread.detach();
    }

    closesocket(ListenSocket);
    WSACleanup();

    return 0;
}