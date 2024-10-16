// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actors/FuseBox.h"
#include "../NetworkingThread.h"
#include "Manager/Main.h"
#include "Manager/MyGameInstance.h"
#include <concurrent_queue.h>
#include "../../../../../Server/Single Thread ASIO/source/Protocol.h"
#include "../../../../../Server/Lobby Server/protocol.h"
#include "FuseBoxManager.generated.h"

UCLASS()
class AFuseBoxManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFuseBoxManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void InitFuseBox();
	void ActiveFuseBox(int idx);
	void SetCompleteFuseBox(SC_FUSE_BOX_ACTIVE_PACKET packet);
	void SaveFuseBoxProgressRatio(SC_OPENING_FUSE_BOX_PACKET packet);
	void PlayOpenedFuseBoxAnim(SC_FUSE_BOX_OPENED_PACKET packet);
	//void ResetFuseBoxStatus(SC_RESET_FUSE_BOX_PACKET packet);
	void StopOpeningFuseBox(SC_STOP_OPENING_PACKET packet);

	void Set_FuseBox_Active_Queue(SC_FUSE_BOX_ACTIVE_PACKET* packet);
	void Set_FuseBox_Opened_Queue(SC_FUSE_BOX_OPENED_PACKET* packet);
	void Set_FuseBox_Opening_Queue(SC_OPENING_FUSE_BOX_PACKET* packet);
	//void Set_FuseBox_Reset_Queue(SC_RESET_FUSE_BOX_PACKET* packet);
	void Set_Stop_Opening_Queue(SC_STOP_OPENING_PACKET* packet);
	
	AFuseBox* GetFuseBoxInArraybyIndex(int idx);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FuseBox")
	AFuseBox* fuseBoxActor;
	TArray<AActor*> FuseBoxes;
private:
	UMyGameInstance* GameInstance = nullptr;
	FSocketThread* Network;

	concurrency::concurrent_queue <SC_FUSE_BOX_ACTIVE_PACKET> FuseBox_Active_Queue;
	concurrency::concurrent_queue <SC_OPENING_FUSE_BOX_PACKET> FuseBox_Opening_Queue;
	concurrency::concurrent_queue <SC_FUSE_BOX_OPENED_PACKET> FuseBox_Opened_Queue;
	concurrency::concurrent_queue <SC_STOP_OPENING_PACKET> FuseBox_Stop_Opening_Queue;
	//concurrency::concurrent_queue <SC_RESET_FUSE_BOX_PACKET> FuseBox_Reset_Queue;

};
