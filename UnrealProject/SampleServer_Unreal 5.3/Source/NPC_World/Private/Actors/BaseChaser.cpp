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
	ProcessCustomEvent(FName("AtkAnimEvent"));
}

void ABaseChaser::PlayResetFirstPersonAnimation()
{
	ProcessCustomEvent(FName("ResetFuseBoxFirstPersonAnimEvent"));
}
void ABaseChaser::PlayResetThirdPersonAnimation()
{
	ProcessCustomEvent(FName("ResetFuseBoxThirdPersonAnimEvent"));
}

void ABaseChaser::ProcessCustomEvent(FName FuncName)
{
	UFunction* CustomEvent = FindFunction(FuncName);
	if (CustomEvent) {
		ProcessEvent(CustomEvent, nullptr);
	}
}
bool ABaseChaser::IsFacingFuseBox(AFuseBox* FacingFuseBox)
{
	if (!FacingFuseBox) return false;

	FVector PlayerForwardVector = GetActorForwardVector();
	FVector FuseBoxRightVector = FacingFuseBox->GetActorRightVector();
	PlayerForwardVector.Z = 0;
	FuseBoxRightVector.Z = 0;

	PlayerForwardVector.Normalize();
	FuseBoxRightVector.Normalize();

	float DotProduct = FVector::DotProduct(PlayerForwardVector, FuseBoxRightVector);

	return FMath::Abs(DotProduct) > 0.98f;
}

FHitResult ABaseChaser::PerformLineTrace(FVector CameraLocation, FRotator CameraRotation, float distance)
{
	FVector ShotDirection = CameraRotation.Vector();
	FVector TraceEnd = CameraLocation + (ShotDirection * distance);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;

	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, CameraLocation, TraceEnd, ECC_Visibility, QueryParams);
	return Hit;
}
bool ABaseChaser::FindFuseBoxInView(FVector CameraLocation, FRotator CameraRotation, float distance)
{
	FHitResult Hit = PerformLineTrace(CameraLocation, CameraRotation, distance);
	AFuseBox* HitFuseBox = Cast<AFuseBox>(Hit.GetActor());
	UDataUpdater* local_DataUpdater = Cast<UDataUpdater>(GetComponentByClass(UDataUpdater::StaticClass()));
	if (!local_DataUpdater) return false;
	if (HitFuseBox && IsFacingFuseBox(HitFuseBox)) {
		bool fuseBoxOpen;
		FuseBox = HitFuseBox;
		HitFuseBox->GetOpenedStatus(fuseBoxOpen);

		if (HitFuseBox->CheckFuseBoxActivate()) {
			ProcessCustomEvent(FName("HideUI"));
			local_DataUpdater->SetFuseBoxOpenAndInstall(-1);
		}
		else if (fuseBoxOpen) {
			ProcessCustomEvent(FName("ShowResetFuseBoxUI"));
			int idx = HitFuseBox->GetIndex();
			local_DataUpdater->SetFuseBoxOpenAndInstall(idx);
		}
		else if (HitFuseBox->GetFuseBoxCurrentProgress()) {
			ProcessCustomEvent(FName("ShowResetFuseBoxUI"));
			int idx = HitFuseBox->GetIndex();
			local_DataUpdater->SetFuseBoxOpenAndInstall(idx);
		}
		else {
			ProcessCustomEvent(FName("HideUI"));
			local_DataUpdater->SetFuseBoxOpenAndInstall(-1);
		}
	}
	else {
		ProcessCustomEvent(FName("HideUI"));
		//ProcessCustomEvent(FName("SendStopInteractionPAcket"));
		local_DataUpdater->SetFuseBoxOpenAndInstall(-1);
		FuseBox = nullptr;
		return false;
	}
	return true;
}
