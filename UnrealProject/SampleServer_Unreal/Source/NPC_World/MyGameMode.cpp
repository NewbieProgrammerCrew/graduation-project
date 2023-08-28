// Fill out your copyright notice in the Description page of Project Settings.

#include "Engine/Engine.h" // 이 헤더를 추가합니다.

#include "MyGameMode.h"

AMyGameMode::AMyGameMode()
{

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

}
void AMyGameMode::BeginPlay() {
	Super::BeginPlay();
	TCPClient = NewObject<UMyTCPClient>(this);

	if (TCPClient) {
		if (TCPClient->ConnectToServer(TEXT("127.0.0.1"), 8080))
		{

		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Hello"));
}
void AMyGameMode::SpawnDefaultPawnFor(AController* NewPlayer, AActor* StartSpot)
{
	Super::SpawnDefaultPawnFor(NewPlayer, StartSpot);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Hello"));
}