// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyTCPClient.generated.h"

/**
 * 
 */
UCLASS()
class NPC_WORLD_API UMyTCPClient : public UObject
{
	GENERATED_BODY()
  

public:
    UMyTCPClient();

    // 서버에 연결하는 함수
    bool ConnectToServer(const FString& IPAddress, int32 Port);

    // 데이터를 서버로 보내는 함수
    void SendData(const FString& Data);

    // 서버에서 데이터를 받는 함수 (예시로 작성)
    FString ReceiveData();
    FSocket* ClientSocket;


    UFUNCTION(BlueprintCallable, Category = "Networking")
    void SendPlayerPosition(const FVector& Position);

    UFUNCTION(BlueprintCallable, Category = "Networking")
    FVector ReceivePlayerPosition();
};