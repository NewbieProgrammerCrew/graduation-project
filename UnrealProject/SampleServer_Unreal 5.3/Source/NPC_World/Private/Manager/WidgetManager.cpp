// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/WidgetManager.h"

// Sets default values
AWidgetManager::AWidgetManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWidgetManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWidgetManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWidgetManager::CallBindSelectRoleWidget()
{
	UFunction* AddSelectRoleWidget = FindFunction(FName("AddSelectRoleWidget"));
	if (AddSelectRoleWidget) {
		ProcessEvent(AddSelectRoleWidget, nullptr);
	}
}

void AWidgetManager::CallBindSelectCharacterWidget()
{
	UFunction* AddSelectCharacterWidget = FindFunction(FName("AddSelectCharacterWidget"));
	if (AddSelectCharacterWidget) {
		ProcessEvent(AddSelectCharacterWidget, nullptr);
	}
}

