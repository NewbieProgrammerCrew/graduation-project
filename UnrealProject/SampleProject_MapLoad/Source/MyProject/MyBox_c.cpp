// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBox_c.h"

// Sets default values
AMyBox_c::AMyBox_c()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));

	RootComponent = MeshComponent;
}

// Called when the game starts or when spawned
void AMyBox_c::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyBox_c::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

