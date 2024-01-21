// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actors/ItemBox.h"
#include <concurrent_queue.h>

#include "../../../../../../Server/ServerTest/ServerTest/protocol.h"
#include "../NetworkingThread.h"
#include "Manager/MyGameInstance.h"
#include "ItemBoxManager.generated.h"

UCLASS()
class NPC_WORLD_API AItemBoxManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AItemBoxManager();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
	void OpenItemBox(int idx, int gun_id);
	void ClosedItemBox(int idx);
	void Set_SwapGun(SC_PICKUP_GUN_PACKET* packet);
	void SwapGun(SC_PICKUP_GUN_PACKET packet);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemBox")
	TArray<AItemBox*> ItemBoxes;
	UMyGameInstance* GameInstance = nullptr;
	FSocketThread* Network;
private:
	concurrency::concurrent_queue <SC_PICKUP_GUN_PACKET> ItemBox_SwapGun;
};
