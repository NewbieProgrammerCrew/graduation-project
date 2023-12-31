// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseChaser.h"
#include "Animation/AnimInstance.h" 
#include "Animation/AnimMontage.h"

// Sets default values
ABaseChaser::ABaseChaser()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseChaser::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseChaser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseChaser::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
void ABaseChaser::Attack()
{
	FSoftObjectPath MontagePath(TEXT("/Game/Animation/Abong/MTG_Attack.MTG_Attack"));
	UAnimMontage* AttackMontage = Cast<UAnimMontage>(MontagePath.TryLoad());
	if (AttackMontage) {
		FName StartSectionName = "Attack";
		PlayAnimMontage(AttackMontage, 1.0f, StartSectionName);
	}
}
