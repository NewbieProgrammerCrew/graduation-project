// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PreviewCharacter.h"

// Sets default values
APreviewCharacter::APreviewCharacter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mySkeletonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	RootComponent = mySkeletonMeshComponent;
}

// Called when the game starts or when spawned
void APreviewCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APreviewCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APreviewCharacter::ApplyRunnerCharacterSkeletonMesh(int characterType)
{
	GenerateRunnerSkeletonMesh(characterType);
	SetActorScale3D(FVector(15, 15, 15));
}
void APreviewCharacter::ApplyChaserCharacterSkeletonMesh(int characterType)
{	
	GenerateChaserSkeletonMesh(characterType);
	SetActorScale3D(FVector(2.f, 2.f, 2.f));
}

void APreviewCharacter::GenerateRandomSkeletonMesh()
{
	int characterType = rand() % 7 + 1;
	if (characterType < 6){
		GenerateRunnerSkeletonMesh(characterType);
		SetActorScale3D(FVector(20, 20, 20));
	}
	else {
		GenerateChaserSkeletonMesh(characterType);
		SetActorScale3D(FVector(1.5, 1.5, 1.5));
	}
	
}

void APreviewCharacter::GenerateRunnerSkeletonMesh(int characterType)
{
	mySkeletonMeshComponent->SetSkeletalMesh(RunnerSkeleltonMesh);
	int idx = characterType - 1;
	if (Materials[idx]) {
		UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(Materials[idx], this);
		if (DynamicMaterialInstance) {
			mySkeletonMeshComponent->SetMaterial(0, DynamicMaterialInstance);
		}
	}
	mySkeletonMeshComponent->PlayAnimation(RunnerIdleAnimation, true);
}
void APreviewCharacter::GenerateChaserSkeletonMesh(int characterType)
{
	mySkeletonMeshComponent->SetSkeletalMesh(ChaserSkeleltonMesh);
	int idx = characterType - 1;
	if (Materials[idx]) {
		UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(Materials[idx], this);
		if (DynamicMaterialInstance) {
			mySkeletonMeshComponent->SetMaterial(0, DynamicMaterialInstance);
		}
	}
	mySkeletonMeshComponent->PlayAnimation(ChaserIdleAnimation, true);
}
