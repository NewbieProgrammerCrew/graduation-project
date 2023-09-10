// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerManager.h"
#include <string>
#include <sstream>
#include "MyPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Main.h"
#include "NetworkingThread.h"
#include "DataUpdater.h"

APlayerManager::APlayerManager()
{
	PrimaryActorTick.bCanEverTick = true;
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
	Network = nullptr;
}

void APlayerManager::BeginPlay()
{
	Super::BeginPlay();

}


void APlayerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Network == nullptr) {
		Network = reinterpret_cast<FSocketThread*>(Main->Network);
		Network->_PlayerManager = this;
		UE_LOG(LogTemp, Log, TEXT("Manager connect"));
	}
	SC_ADD_PLAYER_PACKET AddPlayer;
	while (!PlayerQueue.empty()) {
		if (PlayerQueue.try_pop(AddPlayer)) {
			Spawn_Player(AddPlayer);
		}
	}
	SC_MOVE_PLAYER_PACKET move_player;
	while (!Player_Move_Queue.empty()) {
		if (Player_Move_Queue.try_pop(move_player)) {
			FRotator Rotation = FRotator(move_player.rx, move_player.ry, move_player.rz);
			FVector location = FVector(move_player.x, move_player.y, move_player.z);
			cur_speed = move_player.speed;
			Set_Player_Location(move_player.id, location, Rotation);
		}
	}

	SC_REMOVE_PLAYER_PACKET remove_player;
	while (!Player_Remove_Queue.empty()) {
		if (Player_Remove_Queue.try_pop(remove_player)) {
			Remove_Player(remove_player.id);
		}
	}
}

void APlayerManager::Spawn_Player(SC_ADD_PLAYER_PACKET AddPlayer)
{
	UWorld* uworld = GetWorld();
	APawn* SpawnedPlayer = nullptr;

	if (AddPlayer.id >= 0 && Player[AddPlayer.id] == nullptr) {
		SpawnedPlayer = uworld->SpawnActor<APawn>(PlayerBP, FVector(0, 0, 0), FRotator(0.0f, 0.0f, 0.0f));
		if (SpawnedPlayer)	Player[AddPlayer.id] = SpawnedPlayer;

		if (AddPlayer.id == Network->my_id) {
			APlayerController* RawController = UGameplayStatics::GetPlayerController(this, 0);
			AMyPlayerController* MyController = Cast<AMyPlayerController>(RawController);
			if (MyController) {
				MyController->Possess(SpawnedPlayer);
			}
		}
	}
	else {
		Player[AddPlayer.id]->SetActorHiddenInGame(false);
		Player[AddPlayer.id]->SetActorLocation(FVector(0, 0, 0));
	}
	if (Player[AddPlayer.id] != nullptr) {
		Player[AddPlayer.id]->SetActorHiddenInGame(false);
	}
}

void APlayerManager::Set_Player_Location(int _id, FVector Packet_Location, FRotator Rotate)
{
	if (_id >= 0 && Player[_id] != nullptr) {
		if (Player[_id]->GetWorld() && Player[_id]->IsValidLowLevel()) {
			UDataUpdater* DataUpdater = Cast<UDataUpdater>(Player[_id]->GetComponentByClass(UDataUpdater::StaticClass()));
			if (DataUpdater) {
				DataUpdater->UpdateSpeedData(cur_speed);
			}
			Player[_id]->SetActorLocation(Packet_Location);
			Player[_id]->SetActorRotation(Rotate);
		}
	}
}
void APlayerManager::Remove_Player(int _id)
{
	if (Player[_id] != nullptr) {
		Player[_id]->Destroy();
		Player[_id] = nullptr;
	}
}

void APlayerManager::SetPlayerQueue(SC_ADD_PLAYER_PACKET* packet)
{
	PlayerQueue.push(*packet);
}
void APlayerManager::Set_Player_Move_Queue(SC_MOVE_PLAYER_PACKET* packet)
{
	Player_Move_Queue.push(*packet);
}
void APlayerManager::Set_Player_Remove_Queue(SC_REMOVE_PLAYER_PACKET* packet)
{
	Player_Remove_Queue.push(*packet);
}