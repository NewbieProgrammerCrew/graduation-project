// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Actors/FuseBox.h"

// Sets default values
AFuseBox::AFuseBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	index = 0;
}

// Called when the game starts or when spawned
void AFuseBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFuseBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int AFuseBox::GetIndex() const
{
	return index;
}

