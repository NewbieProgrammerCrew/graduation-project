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

void ABaseChaser::UpdateTransform(FRotator rotator, FVector Pos)
{
	m_rotator = rotator;
	m_pos = Pos;
}

void ABaseChaser::GetUpdatedTransform(FRotator& rotator, FVector& Pos)
{
	rotator = m_rotator;
	Pos = m_pos;
}
// Called to bind functionality to input
void ABaseChaser::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
void ABaseChaser::Attack()
{
	UFunction* AttackEvent = FindFunction(FName("AtkAnimEvent"));
	if (AttackEvent) {
		ProcessEvent(AttackEvent, nullptr);
	}
}
