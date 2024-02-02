// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerComponents/Cpt_IntersectionIK.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components\CapsuleComponent.h"
#include "DrawDebugHelpers.h"

UCpt_IntersectionIK::UCpt_IntersectionIK()
{
	PrimaryComponentTick.bCanEverTick = true;

	m_fIKFeetInterpSpeed = 12.0f;
	m_fIKHipsInterpSpeed = 8.0f;
	m_fIKTraceDistance = 30.0f;
	m_IKAdjustOffset = 0.0f;

	m_bDebug = false;
}


// Called when the game starts
void UCpt_IntersectionIK::BeginPlay()
{
	Super::BeginPlay();
	Character = Cast<ACharacter>(GetOwner());
	if (!Character) return;
	CapsuleHalkHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	SetIKActive(true);
}


// Called every frame
void UCpt_IntersectionIK::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!Character) return;
	if (!m_bActive) return;
	IK_Update(DeltaTime);
}

void UCpt_IntersectionIK::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Character = nullptr;
}

void UCpt_IntersectionIK::IK_Update(float deltaTime)
{
	m_fDeltaTime = deltaTime;
	Intersection_ST_IK_TraceInfo pTrace_Left = IK_HandTrace(m_fIKTraceDistance, m_sIKSocketName_LeftHand);
	Intersection_ST_IK_TraceInfo pTrace_Right = IK_HandTrace(m_fIKTraceDistance, m_sIKSocketName_RightHand);

	if (!IsMoving()) {
		IK_Update_HandRotation(m_fDeltaTime, NormalToRotator(pTrace_Left.pImpactLocation), &m_Intersection_stIKAnimValuse.m_pHandRotation_Left, m_fIKFeetInterpSpeed);
		IK_Update_HandRotation(m_fDeltaTime, NormalToRotator(pTrace_Right.pImpactLocation), &m_Intersection_stIKAnimValuse.m_pHandRotation_Right, m_fIKFeetInterpSpeed);
	}
	else {
		IK_Update_HandRotation(m_fDeltaTime, FRotator::ZeroRotator, &m_Intersection_stIKAnimValuse.m_pHandRotation_Right, m_fIKFeetInterpSpeed);
		IK_Update_HandRotation(m_fDeltaTime, FRotator::ZeroRotator, &m_Intersection_stIKAnimValuse.m_pHandRotation_Left, m_fIKFeetInterpSpeed);
	}

	float fHipsOffset = UKismetMathLibrary::Min(pTrace_Left.fOffset, pTrace_Right.fOffset);
	if (fHipsOffset < 0.0f == false) fHipsOffset = 0.0f;
	IK_Update_HandOffset(m_fDeltaTime, fHipsOffset, &m_Intersection_stIKAnimValuse.m_fOffset, m_fIKHipsInterpSpeed);
	IK_Update_CapsuleHalfHeight(m_fDeltaTime, fHipsOffset, false);
	IK_Update_HandOffset(m_fDeltaTime, pTrace_Left.fOffset - fHipsOffset, &m_Intersection_stIKAnimValuse.m_fEffectorLocation_Left, m_fIKFeetInterpSpeed);
	IK_Update_HandOffset(m_fDeltaTime, -1 * (pTrace_Right.fOffset - fHipsOffset), &m_Intersection_stIKAnimValuse.m_fEffectorLocation_Right, m_fIKFeetInterpSpeed);

}

Intersection_ST_IK_TraceInfo UCpt_IntersectionIK::IK_HandTrace(float fTraceDistance, FName sSocket)
{
	Intersection_ST_IK_TraceInfo TraceInfo;
	FVector SocketLocation = Character->GetMesh()->GetSocketLocation(sSocket);
	FVector LineStart = FVector(SocketLocation.X, SocketLocation.Y, SocketLocation.Z + CapsuleHalkHeight);
	FVector LineEnd = FVector(SocketLocation.X, SocketLocation.Y, (SocketLocation.Z) - fTraceDistance);

	FHitResult HitResult;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetOwner());

	bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), LineStart, LineEnd,
		UEngineTypes::ConvertToTraceType(ECC_Visibility), true, IgnoreActors, EDrawDebugTrace::None, HitResult, true);

	if (m_bDebug) {
		FColor DebugColor = bHit ? FColor::Green : FColor::Red;
		DrawDebugLine(GetWorld(), LineStart, LineEnd, DebugColor, false, 5.0f, (uint8)'\000', 1.0f);
	}

	TraceInfo.pImpactLocation = HitResult.ImpactNormal;
	if (bHit && HitResult.IsValidBlockingHit()) {
		float fImpactLegth = (HitResult.ImpactPoint - HitResult.TraceEnd).Size();
		TraceInfo.fOffset = m_IKAdjustOffset + (fImpactLegth - fTraceDistance);
	}
	else {
		TraceInfo.fOffset = 0.0f;
	}
	return TraceInfo;
}

FRotator UCpt_IntersectionIK::NormalToRotator(FVector pVector)
{
	return FRotator();
}

void UCpt_IntersectionIK::IK_Update_CapsuleHalfHeight(float fDeltaTime, float fHipsShifts, bool bResetDefault)
{

}

void UCpt_IntersectionIK::IK_Update_HandOffset(float fDeltaTime, float fTargetValue, float* fEffectorValue, float finterpSpeed)
{

}

void UCpt_IntersectionIK::IK_Update_HandRotation(float fDeltaTime, FRotator pTargetValue, FRotator* pHandRotatorValue, float finterpSpeed)
{

}

bool UCpt_IntersectionIK::IsMoving()
{
	return false;
}


void UCpt_IntersectionIK::SetIKActive(bool bActive)
{
	m_bActive = true;
}
