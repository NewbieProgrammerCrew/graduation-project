// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Manager/FuseBoxManager.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AFuseBoxManager::AFuseBoxManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Network = nullptr;
}

// Called when the game starts or when spawned
void AFuseBoxManager::BeginPlay()
{
	Super::BeginPlay();
	GameInstance = Cast<UMyGameInstance>(GetGameInstance());

	for (const auto& fusebox : FuseBoxes) {
		fusebox->SetActorEnableCollision(false);
		auto MeshArray = fusebox->GetMeshComponent();
		for (const auto& mesh:MeshArray) {
			mesh->SetVisibility(false);
		}
	}

	TArray<int> ActiveIdx = GameInstance->GetActiveFuseBoxIndex();
	for (int idx : ActiveIdx) {
		ActiveFuseBox(idx);
	}
}

// Called every frame
void AFuseBoxManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!Network && !GameInstance->Network) {
		Network = GameInstance->Network;
		if (!GameInstance->Network->_FuseBoxManager) {
			GameInstance->Network->_FuseBoxManager = this;
		}
	}
}
// 0 : fuseBox
// 1 : fuseBox Cover
// 2 : fuse
void AFuseBoxManager::ActiveFuseBox(int idx)
{
	FuseBoxes[idx]->SetActorEnableCollision(true);
	auto MeshArray = FuseBoxes[idx]->GetMeshComponent();
	MeshArray[0]->SetVisibility(true);
	MeshArray[1]->SetVisibility(true);
}

