// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerComponents/Cpt_FootIK.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components\CapsuleComponent.h"
#include "DrawDebugHelpers.h"

UCpt_FootIK::UCpt_FootIK()
{
	PrimaryComponentTick.bCanEverTick = true;

    m_fIKFeetInterpSpeed = 12.0f;
    m_fIKHipsInterpSpeed = 8.0f;
    m_fIKTraceDistance = 30.0f;
    m_IKAdjustOffset = 0.0f;

    m_bDebug = false;
}


// Called when the game starts
void UCpt_FootIK::BeginPlay()
{
	Super::BeginPlay();	
    Character = Cast<ACharacter>(GetOwner());
    if (!Character) return;
    CapsuleHalkHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    SetIKActive(true);
}

void UCpt_FootIK::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
    Character = nullptr;
}



// Called every frame
void UCpt_FootIK::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (!Character) return;
    if (!m_bActive) return;
    IK_Update(DeltaTime);
}

void UCpt_FootIK::Set_IKSocketName(FString leftFootSocket, FString RightFootSocket)
{
}

void UCpt_FootIK::IK_ResetVars()
{
}

void UCpt_FootIK::SetIKActive(bool bActive)
{
    m_bActive = true;
}

void UCpt_FootIK::IK_Update(float deltaTime)
{
    m_fDeltaTime = deltaTime;
    ST_IK_TraceInfo pTrace_Left = IK_FootTrace(m_fIKTraceDistance, m_sIKSocketName_LeftFoot);
    ST_IK_TraceInfo pTrace_Right = IK_FootTrace(m_fIKTraceDistance, m_sIKSocketName_RightFoot);

    if (!IsMoving()) {
        IK_Update_FootRotation(m_fDeltaTime, NormalToRotator(pTrace_Left.pImpactLocation), &m_stIKAnimValuse.m_pFootRotation_Left, m_fIKFeetInterpSpeed);
        IK_Update_FootRotation(m_fDeltaTime, NormalToRotator(pTrace_Right.pImpactLocation), &m_stIKAnimValuse.m_pFootRotation_Right, m_fIKFeetInterpSpeed);
    }
    else {
        IK_Update_FootRotation(m_fDeltaTime, FRotator::ZeroRotator, &m_stIKAnimValuse.m_pFootRotation_Right, m_fIKFeetInterpSpeed);
        IK_Update_FootRotation(m_fDeltaTime, FRotator::ZeroRotator, &m_stIKAnimValuse.m_pFootRotation_Left, m_fIKFeetInterpSpeed);
    }

    float fHipsOffset = UKismetMathLibrary::Min(pTrace_Left.fOffset, pTrace_Right.fOffset);
    if (fHipsOffset < 0.0f == false) fHipsOffset = 0.0f;
    IK_Update_FootOffset(m_fDeltaTime, fHipsOffset, &m_stIKAnimValuse.m_fHipOffset, m_fIKHipsInterpSpeed);
    IK_Update_CapsuleHalfHeight(m_fDeltaTime, fHipsOffset, false);
    IK_Update_FootOffset(m_fDeltaTime, pTrace_Left.fOffset - fHipsOffset, &m_stIKAnimValuse.m_fEffectorLocation_Left, m_fIKFeetInterpSpeed);
    IK_Update_FootOffset(m_fDeltaTime, -1 * (pTrace_Right.fOffset - fHipsOffset), &m_stIKAnimValuse.m_fEffectorLocation_Right, m_fIKFeetInterpSpeed);
}

ST_IK_TraceInfo UCpt_FootIK::IK_FootTrace(float fTraceDistance, FName sSocket)
{
    ST_IK_TraceInfo TraceInfo;
    FVector SocketLocation = Character->GetMesh()->GetSocketLocation(sSocket);
    FVector LineStart = FVector(SocketLocation.X, SocketLocation.Y, Character->GetActorLocation().Z);
    FVector LineEnd = FVector(SocketLocation.X, SocketLocation.Y, (Character->GetActorLocation().Z - CapsuleHalkHeight) - fTraceDistance);

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
void UCpt_FootIK::IK_Update_CapsuleHalfHeight(float fDeltaTime, float fHipsShifts, bool bResetDefault)
{
    UCapsuleComponent* pCapsule = Character->GetCapsuleComponent();
    if (pCapsule == nullptr) {
        UE_LOG(LogTemp, Warning, TEXT("IK : Capsule is NULL"));
        return;
    }

    float fCapsuleHalf = 0.0f;
    if (bResetDefault == true) {
        fCapsuleHalf = CapsuleHalkHeight;
    }
    else {
        float fHalfAbsSize = UKismetMathLibrary::Abs(fHipsShifts) * 0.5f;
        fCapsuleHalf = CapsuleHalkHeight - fHalfAbsSize;
    }
    float fScaledCapsuleHalfHeight = pCapsule->GetScaledCapsuleHalfHeight();
    float fInterpValue = UKismetMathLibrary::FInterpTo(fScaledCapsuleHalfHeight, fCapsuleHalf, fDeltaTime, m_fIKHipsInterpSpeed);

    pCapsule->SetCapsuleHalfHeight(fInterpValue, true);
}

FRotator UCpt_FootIK::NormalToRotator(FVector pVector)
{
    float fAtan2_1 = UKismetMathLibrary::DegAtan2(pVector.Y, pVector.Z);
    float fAtan2_2 = UKismetMathLibrary::DegAtan2(pVector.X, pVector.Z);
    fAtan2_2 *= -1.f;
    FRotator pResult = FRotator(fAtan2_2, 0.f, fAtan2_1);
    return pResult;
}

void UCpt_FootIK::IK_Update_FootOffset(float fDeltaTime, float fTargetValue, float* fEffectorValue, float finterpSpeed)
{
    float finterpValue = UKismetMathLibrary::FInterpTo(*fEffectorValue, fTargetValue, fDeltaTime, finterpSpeed);
    *fEffectorValue = finterpValue;
}

void UCpt_FootIK::IK_Update_FootRotation(float fDeltaTime, FRotator pTargetValue, FRotator* pFootRotatorValue, float finterpSpeed)
{
    FRotator pInterpRotator = UKismetMathLibrary::RInterpTo(*pFootRotatorValue, pTargetValue, fDeltaTime, finterpSpeed);
    *pFootRotatorValue = pInterpRotator;
}

bool UCpt_FootIK::IsMoving()
{
    UDataUpdater* DataUpdater = Cast<UDataUpdater>(Character->GetComponentByClass(UDataUpdater::StaticClass()));
    float speed = DataUpdater->GetCurrentSpeed();
    if (speed > 0)return true;
    return false;
}
