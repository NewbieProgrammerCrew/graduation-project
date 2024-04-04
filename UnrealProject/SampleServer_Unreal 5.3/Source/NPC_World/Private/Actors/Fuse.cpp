// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Fuse.h"

// Sets default values
AFuse::AFuse()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFuse::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFuse::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFuse::SetId(int nid)
{
	id = nid;
}

int AFuse::GetId()
{
	return id;
}

