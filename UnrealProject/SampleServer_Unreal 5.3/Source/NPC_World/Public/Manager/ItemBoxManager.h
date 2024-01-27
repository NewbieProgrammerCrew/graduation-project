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
	virtual void Tick(float DeltaTime) override;
public:	
	void Set_SwapGun(SC_PICKUP_GUN_PACKET* packet);
	void Set_OpenBox(SC_ITEM_BOX_OPENED_PACKET* packet);
	void ClosedItemBox(int idx);
	void SwapGun(SC_PICKUP_GUN_PACKET packet);
	void OpenItemBox(SC_ITEM_BOX_OPENED_PACKET packet);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemBox")
	TArray<AItemBox*> ItemBoxes;
	UMyGameInstance* GameInstance = nullptr;
	FSocketThread* Network;
private:
	concurrency::concurrent_queue <SC_PICKUP_GUN_PACKET> ItemBox_SwapGun;
	concurrency::concurrent_queue <SC_ITEM_BOX_OPENED_PACKET> Open_ItemBox;
};
