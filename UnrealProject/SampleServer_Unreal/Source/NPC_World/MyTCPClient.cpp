// Fill out your copyright notice in the Description page of Project Settings.


#include "MyTCPClient.h"
#include "Engine/Engine.h"
#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

UMyTCPClient::UMyTCPClient()
{
    // Constructor logic if needed
}

bool UMyTCPClient::ConnectToServer(const FString& IPAddress, int32 Port)
{
    // Get the socket subsystem
    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

    FIPv4Address Addr;
    if (FIPv4Address::Parse(IPAddress, Addr)) {
        FIPv4Endpoint Endpoint(Addr, Port);
        TSharedRef<FInternetAddr> Address = SocketSubsystem->CreateInternetAddr();
        Address->SetIp(Addr.Value);
        Address->SetPort(Port);

        // Create the socket with the specified type and protocol
        FSocket* Socket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("TCPClientSocket"), false);

        // Connect to the server
        bool bConnected = Socket->Connect(*Address);
        if (bConnected)
        {
            // Optional: store the socket in a class member for future use
            this->ClientSocket = Socket;
        }
        else
        {
            // Cleanup on failure
            Socket->Close();
            ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
        }

        return bConnected;
    }
    else
        return false;
}

void UMyTCPClient::SendData(const FString& Data)
{
    if (ClientSocket && ClientSocket->GetConnectionState() == ESocketConnectionState::SCS_Connected)
    {
        TArray<uint8> SendData;
        SendData.Append((uint8*)TCHAR_TO_UTF8(*Data), Data.Len());
        int32 BytesSent = 0;
        ClientSocket->Send(SendData.GetData(), SendData.Num(), BytesSent);
    }
}

FString UMyTCPClient::ReceiveData()
{
    FString Result;
    if (ClientSocket && ClientSocket->GetConnectionState() == ESocketConnectionState::SCS_Connected)
    {
        uint32 Size;
        while (ClientSocket->HasPendingData(Size))
        {
            TArray<uint8> ReceivedData;
            ReceivedData.SetNumUninitialized(FMath::Min(Size, 65507u));

            int32 Read = 0;
            ClientSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);

            // Convert data to a FString and append to our result string
            Result += FString(UTF8_TO_TCHAR(ReceivedData.GetData()));
        }
    }
 

    return Result;
}

void UMyTCPClient::SendPlayerPosition(const FVector& Position)
{

    if (ClientSocket && ClientSocket->GetConnectionState() == ESocketConnectionState::SCS_Connected)
    {
     
        FString Data = FString::Printf(TEXT("%f,%f,%f"), Position.X, Position.Y, Position.Z);
        SendData(Data);
    }
}

FVector UMyTCPClient::ReceivePlayerPosition()
{
    FVector Result = FVector::ZeroVector;
    FString Data = ReceiveData();

    TArray<FString> Parsed;
    Data.ParseIntoArray(Parsed, TEXT(","), true);
    if (Parsed.Num() == 3)
    {
        Result.X = FCString::Atof(*Parsed[0]);
        Result.Y = FCString::Atof(*Parsed[1]);
        Result.Z = FCString::Atof(*Parsed[2]);
    }

    return Result;
}