// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/ItemBoxManager.h"

// Sets default values
AItemBoxManager::AItemBoxManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItemBoxManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemBoxManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!Network && GameInstance->Network) {
		Network = GameInstance->Network;
		if (!GameInstance->Network->_ItemBoxManager) {
			GameInstance->Network->_ItemBoxManager = this;
		}
	}
}

void AItemBoxManager::OpenItemBox(int idx, int gun_id)
{
	if (idx < ItemBoxes.Num() && ItemBoxes[idx]) {
		UFunction* OpenCustomEvent = ItemBoxes[idx]->FindFunction(FName("OpenCustomEvent"));
		if (OpenCustomEvent) {
			ItemBoxes[idx]->ProcessEvent(OpenCustomEvent, nullptr);
		}
	}
}

void AItemBoxManager::ClosedItemBox(int idx)
{
	if (idx < ItemBoxes.Num() && ItemBoxes[idx]) {
		UFunction* CloseCustomEvent = ItemBoxes[idx]->FindFunction(FName("CloseCustomEvent"));
		if (CloseCustomEvent) {
			ItemBoxes[idx]->ProcessEvent(CloseCustomEvent, nullptr);
		}
	}
}

