// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework\Character.h"
#include "DataUpdater.h"
#include "Cpt_IntersectionIK.generated.h"

struct Intersection_ST_IK_TraceInfo {
	float fOffset{};
	FVector pImpactLocation{};
};
USTRUCT(Atomic, BlueprintType)
struct FIST_IK_AnimValue
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float		m_fEffectorLocation_Left;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float		m_fEffectorLocation_Right;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator	m_pHandRotation_Left;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator	m_pHandRotation_Right;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float		m_fOffset;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NPC_WORLD_API UCpt_IntersectionIK : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCpt_IntersectionIK();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void IK_Update(float deltaTime);
	Intersection_ST_IK_TraceInfo IK_HandTrace(float fTraceDistance, FName sSocket);
	FRotator NormalToRotator(FVector pVector);
	void IK_Update_CapsuleHalfHeight(float fDeltaTime, float fHipsShifts, bool bResetDefault);
	void IK_Update_HandOffset(float fDeltaTime, float fTargetValue, float* fEffectorValue, float finterpSpeed);
	void IK_Update_HandRotation(float fDeltaTime, FRotator pTargetValue, FRotator* pHandRotatorValue, float finterpSpeed);
	bool IsMoving();
	
	void SetIKActive(bool bActive); 
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK Values Socket")
	FName m_sIKSocketName_LeftHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK Values Socket")
	FName m_sIKSocketName_RightHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK Values")
	float m_IKAdjustOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK Values")
	float m_fIKTraceDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK Values")
	float m_fIKHipsInterpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK Values")
	float m_fIKFeetInterpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK Debug")
	bool m_bDebug;
public:
	UFUNCTION(BlueprintCallable, Category = "IK")
	FORCEINLINE FIST_IK_AnimValue GetIKAnimValue() const { return m_Intersection_stIKAnimValuse; }
	FORCEINLINE bool GetIKDebugState() { return m_bDebug; }
private:
	FIST_IK_AnimValue m_Intersection_stIKAnimValuse;
	ACharacter* Character{};
	float		CapsuleHalkHeight{};
	bool		m_bActive{};
	float		m_fDeltaTime{};
};
