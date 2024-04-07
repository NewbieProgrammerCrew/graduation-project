// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/FuseManager.h"

// Sets default values
AFuseManager::AFuseManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFuseManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AFuseManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!GameInstance)
		GameInstance = Cast<UMyGameInstance>(GetGameInstance());

	if (!Network && GameInstance && GameInstance->Network) {
		Network = GameInstance->Network;
		if (!GameInstance->Network->_FuseManager) {
			GameInstance->Network->_FuseManager = this;
		}
	}
	SC_PICKUP_FUSE_PACKET destroyFuse;
	while (!Fuse_Destroy_Queue.empty()) {
		if (Fuse_Destroy_Queue.try_pop(destroyFuse)) {
			int idx = destroyFuse.index;
			DestroyFuse(idx);
		}
	}
}

void AFuseManager::AddActiveFuse(int id, AFuse* fuse)
{
	fuse->SetId(id);
	fuses.Add(id, fuse);
}

void AFuseManager::DestroyFuse(int id)
{
	AFuse** FusePtr = fuses.Find(id);
	if (FusePtr && *FusePtr) {
		(*FusePtr)->Destroy();
		fuses.Remove(id);
	}
}

void AFuseManager::Set_Fuse_Destroy_Queue(SC_PICKUP_FUSE_PACKET* packet)
{
	Fuse_Destroy_Queue.push(*packet);
}

