// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actors/ItemBox.h"
#include <concurrent_queue.h>

#include "../../../../../Server/Single Thread ASIO/source/Protocol.h" 
#include "../../../../../Server/Lobby Server/protocol.h"
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
	UFUNCTION(BlueprintCallable)
	void InitItemBox();
	void Set_SwapBomb(SC_PICKUP_BOMB_PACKET* packet);
	void Set_OpenBox(SC_ITEM_BOX_OPENED_PACKET* packet);
	void ClosedItemBox(int idx);
	void SwapBomb(SC_PICKUP_BOMB_PACKET packet);
	void OpenItemBox(SC_ITEM_BOX_OPENED_PACKET packet);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemBox")
	AItemBox* m_itemBox;
	TArray<AActor*> ItemBoxes;
	UMyGameInstance* GameInstance = nullptr;
	FSocketThread* Network;
private:
	concurrency::concurrent_queue <SC_PICKUP_BOMB_PACKET> ItemBox_SwapBomb;
	concurrency::concurrent_queue <SC_ITEM_BOX_OPENED_PACKET> Open_ItemBox;
};
