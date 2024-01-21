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
	if (!GameInstance)
		GameInstance = Cast<UMyGameInstance>(GetGameInstance());

	if(GameInstance)
		if (!Network && GameInstance->Network) {
			Network = GameInstance->Network;
			if (!GameInstance->Network->_ItemBoxManager) {
				GameInstance->Network->_ItemBoxManager = this;
			}
		}

	SC_PICKUP_GUN_PACKET SwapGuninItemBox;
	while (!ItemBox_SwapGun.empty()) {
		if (ItemBox_SwapGun.try_pop(SwapGuninItemBox)) {
			SwapGun(SwapGuninItemBox);
		}
	}
}

void AItemBoxManager::OpenItemBox(int idx, int gun_id)
{
	if (idx < ItemBoxes.Num() && ItemBoxes[idx]) {
		UFunction* OpenCustomEvent = ItemBoxes[idx]->FindFunction(FName("OpenCustomEvent"));
		ItemBoxes[idx]->SetGunItem(gun_id);
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

void AItemBoxManager::Set_SwapGun(SC_PICKUP_GUN_PACKET* packet)
{
	ItemBox_SwapGun.push(*packet);
}

void AItemBoxManager::SwapGun(SC_PICKUP_GUN_PACKET packet)
{
	int idx = packet.itemBoxIndex;
	if (idx < 0) return;
	int leftGunType = packet.leftGunType;
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Yellow, FString::Printf(TEXT("called %d"), leftGunType));
	if (leftGunType < 0) {
		ItemBoxes[idx]->HideGunItem();
	}
	else {
		ItemBoxes[idx]->ShowGunItem();
		ItemBoxes[idx]->SetGunItem(leftGunType);
	}
}

