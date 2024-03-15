// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Bomb.h"

// Sets default values
ABomb::ABomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABomb::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABomb::Throw(FVector Direction, float Force)
{
    TArray<UStaticMeshComponent*> MeshComponents;
    GetComponents<UStaticMeshComponent>(MeshComponents);
    MeshComponents[0]->SetSimulatePhysics(true);
    MeshComponents[0]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    MeshComponents[0]->AddImpulse(Direction * Force);
    MeshComponents[0]->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
	
}

