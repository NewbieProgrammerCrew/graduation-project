// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "NetworkingThread.h"
#include <future>
#include <mutex>
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

}
void AMain::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	//if (Network) Network->Stop();
}
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Main menu
// 1
// 2
// 3