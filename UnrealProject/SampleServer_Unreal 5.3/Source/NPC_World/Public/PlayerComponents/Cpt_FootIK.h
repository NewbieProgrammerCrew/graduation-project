// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework\Character.h"
#include "DataUpdater.h"
#include "Cpt_FootIK.generated.h"

struct ST_IK_TraceInfo {
	float fOffset{};
	FVector pImpactLocation{};
};
USTRUCT(Atomic, BlueprintType)
struct FST_IK_AnimValue
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float		m_fEffectorLocation_Left;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float		m_fEffectorLocation_Right;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator	m_pFootRotation_Left;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator	m_pFootRotation_Right;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float		m_fHipOffset;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NPC_WORLD_API UCpt_FootIK : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCpt_FootIK();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void IK_ResetVars();
	void IK_Update(float deltaTime);
	ST_IK_TraceInfo IK_FootTrace(float fTraceDistance, FName sSocket);
	FRotator NormalToRotator(FVector pVector);
	void IK_Update_CapsuleHalfHeight(float fDeltaTime, float fHipsShifts, bool bResetDefault);
	void IK_Update_FootOffset(float fDeltaTime, float fTargetValue, float* fEffectorValue, float finterpSpeed);
	void IK_Update_FootRotation(float fDeltaTime, FRotator pTargetValue, FRotator* pFootRotatorValue, float finterpSpeed);
	bool IsMoving();
	void Set_IKSocketName(FString leftFootSocket, FString RightFootSocket);
	void SetIKActive(bool bActive);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK Values Socket")
	FName m_sIKSocketName_LeftFoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK Values Socket")
	FName m_sIKSocketName_RightFoot;
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
	FORCEINLINE FST_IK_AnimValue GetIKAnimValue() const { return m_stIKAnimValuse; }
	FORCEINLINE bool GetIKDebugState() { return m_bDebug; }
private:
	FST_IK_AnimValue m_stIKAnimValuse;
	ACharacter*	Character{};
	float		CapsuleHalkHeight{};
	bool		m_bActive{};
	float		m_fDeltaTime{};
};
