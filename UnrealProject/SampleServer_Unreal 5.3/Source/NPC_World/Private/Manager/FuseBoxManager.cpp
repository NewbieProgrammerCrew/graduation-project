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
	UWorld* world = GetWorld();
	if (!world) {
		return;
	}
	AActor* actor = UGameplayStatics::GetActorOfClass(world, AMain::StaticClass());
	if (actor == nullptr) {
		return;
	}
	actor->GetWorld();
	Main = Cast<AMain>(actor);	
	for (const auto& fusebox : FuseBoxes) {
		fusebox->SetActorEnableCollision(false);
		auto MeshArray = fusebox->GetMeshComponent();
		for (const auto& mesh:MeshArray) {
			mesh->SetVisibility(false);
		}
	}
}

// Called every frame
void AFuseBoxManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!Network) {
		if (Main) {
			Network = Main->Network;
			Network->_FuseBoxManager = this;
		}
	}
}
// 0 : fuseBox
// 1 : fuse
// 2 : fuseBox Cover
void AFuseBoxManager::ActiveFuseBox(int idx)
{
	FuseBoxes[idx]->SetActorEnableCollision(true);
	auto MeshArray = FuseBoxes[idx]->GetMeshComponent();
	

}

