// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Actors/JellyTemp.h"

// Sets default values
AJelly::AJelly()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AJelly::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AJelly::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int AJelly::GetIndex() const
{
	return index;
}

void AJelly::ExplosionEffect()
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
			UFunction* ExplosionJellyEvent = FindFunction(FName("ExplosionJellyEvent"));
			if (ExplosionJellyEvent) {
				ProcessEvent(ExplosionJellyEvent, nullptr);
			}
		});
}

