// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PortalGate.h"

// Sets default values
APortalGate::APortalGate()
{
 	PrimaryActorTick.bCanEverTick = true;

}

void APortalGate::BeginPlay()
{
	Super::BeginPlay();
	
}

void APortalGate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APortalGate::OpenPortal()
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		ProcessCustomEvent(FName("OpenPortal"));
		});
}

void APortalGate::CheckPortalStatus()
{
}

void APortalGate::ProcessCustomEvent(FName Name)
{
	UFunction* CustomEvent = FindFunction(Name);
	if (CustomEvent) {
		ProcessEvent(CustomEvent, nullptr);
	}
}
