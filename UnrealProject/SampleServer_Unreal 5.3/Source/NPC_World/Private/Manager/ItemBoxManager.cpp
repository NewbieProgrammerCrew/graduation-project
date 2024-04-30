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

	SC_PICKUP_BOMB_PACKET SwapBombinItemBox;
	while (!ItemBox_SwapBomb.empty()) {
		if (ItemBox_SwapBomb.try_pop(SwapBombinItemBox)) {
			SwapBomb(SwapBombinItemBox);
		}
	}	
	SC_ITEM_BOX_OPENED_PACKET itembox;
	while (!Open_ItemBox.empty()) {
		if (Open_ItemBox.try_pop(itembox)) {
			OpenItemBox(itembox);
		}
	}

}

void AItemBoxManager::OpenItemBox(SC_ITEM_BOX_OPENED_PACKET packet)
{
	int idx = packet.index;
	int Bomb_type = packet.bomb_type;
	if (idx >= 0 && idx < ItemBoxes.Num() && ItemBoxes[idx]) {
		UFunction* OpenCustomEvent = ItemBoxes[idx]->FindFunction(FName("OpenCustomEvent"));
		ItemBoxes[idx]->SetBombItem(Bomb_type);
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

void AItemBoxManager::Set_SwapBomb(SC_PICKUP_BOMB_PACKET* packet)
{
	ItemBox_SwapBomb.push(*packet);
}
void AItemBoxManager::Set_OpenBox(SC_ITEM_BOX_OPENED_PACKET* packet)
{
	Open_ItemBox.push(*packet);
}

void AItemBoxManager::SwapBomb(SC_PICKUP_BOMB_PACKET packet)
{
	int idx = packet.itemBoxIndex;
	if (idx < 0) return;
	int leftBombType = packet.leftBombType;
	if (leftBombType == BombType::NoBomb) {
		ItemBoxes[idx]->HideBombItem();
	}
	else {
		ItemBoxes[idx]->ShowBombItem();
		ItemBoxes[idx]->SetBombItem(leftBombType);
	}
}

