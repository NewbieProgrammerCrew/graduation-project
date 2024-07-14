// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include <concurrent_queue.h>

#include "../Actors/Fuse.h"
#include "../NetworkingThread.h"
#include "Manager/MyGameInstance.h"
#include "../../../../../Server/Single Thread ASIO/source/Protocol.h" 
#include "../../../../../Server/Lobby Server/protocol.h"
#include "FuseManager.generated.h"

UCLASS()
class NPC_WORLD_API AFuseManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFuseManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void AddActiveFuse(int id, AFuse* fuse);
	void DestroyFuse(int id);
	void Set_Fuse_Destroy_Queue(SC_PICKUP_FUSE_PACKET* packet);
private:
	TMap <int, AFuse*> fuses;
	UMyGameInstance* GameInstance = nullptr;
	FSocketThread* Network;
	concurrency::concurrent_queue <SC_PICKUP_FUSE_PACKET> Fuse_Destroy_Queue;
};
