#include "pch.h"
#include <iostream>
#include <thread>

#include <WinSock2.h>
#include <MSWSock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void HandleError(const char* cause)
{
    int32 errCode = ::WSAGetLastError();
    cout << cause << "ErrorCode : " << errCode << endl;
}


int main()
{
    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    SOCKET clientSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        HandleError("Socket");
        return 0;
    }

    // 소켓 리소스 반환
    ::closesocket(clientSocket);

    // 윈속 종료
    ::WSACleanup();
}
