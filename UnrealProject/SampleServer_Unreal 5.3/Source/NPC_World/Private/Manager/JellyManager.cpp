// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/JellyManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"

// Sets default values
AJellyManager::AJellyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GameInstance = nullptr;
	network = nullptr;
}

// Called when the game starts or when spawned
void AJellyManager::BeginPlay()
{
	Super::BeginPlay();
	GameInstance = Cast<UMyGameInstance>(GetGameInstance());
}
// Called every frame
void AJellyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!network && GameInstance->Network) {
		network = GameInstance->Network;
        network->_JellyManager = this;
	}
}

void AJellyManager::ExplosionParticleEvent(int idx)
{
    if(jellies[idx])
    	jellies[idx]->ExplosionEffect();
}

void AJellyManager::LookAtPlayer(ACharacter* Player, int idx)
{
    if (Player) {
        FRotator PlayerRotation = Player->GetActorRotation();
        jellies[idx]->SetActorRotation(PlayerRotation);
    }
}

void AJellyManager::LookAtBomb(FVector bombLocation, int idx)
{
    if (bombLocation == FVector(-999999, -999999, -999999)) return;
    AsyncTask(ENamedThreads::GameThread, [=]() {
        if (!IsValid(jellies[idx])) return; // 유효성 검사 추가

        FVector CurrentLocation = jellies[idx]->GetActorLocation();
        FRotator CurrentRot = GetActorRotation();
        FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, bombLocation);
        FRotator NewRot = FRotator(CurrentRot.Pitch, TargetRot.Yaw + 180.f, CurrentRot.Roll);

        jellies[idx]->SetActorRotation(NewRot);
        });

}


