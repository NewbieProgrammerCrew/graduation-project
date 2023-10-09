// Fill out your copyright notice in the Description page of Project Settings.


#include "Box_C.h"

// Sets default values
ABox_C::ABox_C()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
}

// Called when the game starts or when spawned
void ABox_C::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABox_C::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

