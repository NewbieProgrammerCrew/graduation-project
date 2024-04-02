// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/PlayerController.h"

//character
#include "GameFramework/Character.h"
#include "../../Public/Actors/BaseRunner.h"
#include "../../Public/Actors/BaseChaser.h"

#include "PlayerInputDataAsset.h"
#include "../../Public/PlayerComponents/PacketExchangeComponent.h"
#include "Ch_PlayerController.generated.h"

/**
 * 
 */
class UInputMappingContext;

UCLASS()
class NPC_WORLD_API ACh_PlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ACh_PlayerController(const FObjectInitializer& initialize = FObjectInitializer::Get()) :Super(initialize) {};
	void SetId(int id);
	int GetId();
public:
	void Move(const FInputActionValue& value);
	void MoveEnd(const FInputActionValue& value);
	void SendMovePacket();
	void Sprint(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void StopSprint(const FInputActionValue& value);
	void Attack(const FInputActionValue& value);
	void Interaction(const FInputActionValue& value);
	void InteractionEnd(const FInputActionValue& value);
	void Jump(const FInputActionValue& value);
	void JumpEnd(const FInputActionValue& value);
	void Aiming(const FInputActionValue& value);
	void AimEnd(const FInputActionValue& value);
	void Skill(const FInputActionValue& value);
	void EscapeGame(const FInputActionValue& value);
	void ResetFkey();
	int GetMyID() const { return m_id; }
	void ActivateDashSkill(ABaseChaser* chaser);
	void ResetJumpCount();
	class FSocketThread* Network;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input Status")
	bool F_KeyPressed{};

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UPlayerInputDataAsset* InputActions;
	APawn* ControlledPawn = nullptr;
	int32 jumpCount{};
	float JumpKeyTimeWindow{ 0.5f };
	FTimerHandle TimerHandle_JumpWindow;
	FTimerHandle TimerHandle_SendMovePacket;

	float RunningSpeed = 1800.0f;
	float WalkingSpeed = 600.0f;
	bool keyinput{};
	class AMain* m_Main;
	int m_id;
	bool bSendInteractionPacket{};
	UPacketExchangeComponent* ControlledPawnPacketExchange = nullptr;
};
