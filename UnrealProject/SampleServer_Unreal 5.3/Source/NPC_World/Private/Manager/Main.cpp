// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Manager/Main.h"
#include "NetworkingThread.h"
#include <future>
#include "PlayerController/Ch_PlayerController.h"
#include <Kismet/GameplayStatics.h>

AMain::AMain() 
{
  PrimaryActorTick.bCanEverTick = true;
}
void AMain::SendMapLoadedPacket()
{
	GameInstance->SendMapLoadedPacket();
}
void AMain::BeginPlay()
{
	Super::BeginPlay();
	GameInstance = Cast<UMyGameInstance>(GetGameInstance());
	if (GameInstance->Network == nullptr) {
		GameInstance->SetNetwork();
	}
	Network = GameInstance->Network;
	if (GameInstance->Network->_MainClass == nullptr) {
		GameInstance->Network->_MainClass = this;
	}
	cameraActor = UGameplayStatics::GetActorOfClass(GetWorld(), ACameraActor::StaticClass());
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (cameraActor == nullptr) {
		return;
	}
	localPlayerController = Cast<ACh_PlayerController>(playerController);
}
void AMain::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (LoadedMap) return;
	if(Network && Network->_PlayerManager){
		SendMapLoadedPacket();
		LoadedMap = true;
	}
}

void AMain::ChangeCamera_EscLocCamera()
{
	localPlayerController->SetViewTargetWithBlend(cameraActor);
}

// Main menu
// 1
// 2
// 3