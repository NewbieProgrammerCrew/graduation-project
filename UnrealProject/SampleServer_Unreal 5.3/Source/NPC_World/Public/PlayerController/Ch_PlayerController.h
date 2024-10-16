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
	void SetLobbyId(int id);
	int GetLobbyId();
	void SetGameId(int id);
	int GetGameId();
public:
	void Move(const FInputActionValue& value);
	void MoveEnd(const FInputActionValue& value);
	void SendMovePacket();
	void Sprint(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void StopSprint(const FInputActionValue& value);
	void Attack(const FInputActionValue& value);
	void Interaction(const FInputActionValue& value);
	
	void Debug_Key_Earn_Explosion_Bomb(const FInputActionValue& value);
	void Debug_Key_Earn_Ink_Bomb(const FInputActionValue& value);
	void Debug_Key_Earn_Stun_Bomb(const FInputActionValue& value);
	void Debug_Key_Earn_Portal_Half_Gauge(const FInputActionValue& value);
	void Debug_Earn_Bomb(BombType bombT);
	void InteractionEnd(const FInputActionValue& value);
	void Jump(const FInputActionValue& value);
	void JumpEnd(const FInputActionValue& value);
	void Aiming(const FInputActionValue& value);
	void AimEnd(const FInputActionValue& value);
	void Skill(const FInputActionValue& value);
	void EscapeGame(const FInputActionValue& value);
	void ResetFkey();
	void ResetJumpCount();
	class FSocketThread* Network;
	UFUNCTION(BlueprintCallable)
	void ChangeRideHorse(bool brideHorse) {
		rideHorse = brideHorse;
	}
	UFUNCTION(BlueprintCallable)
	void ChangeLVSequencerStat(bool blvSequencerPlay) {
		lvSequencerPlay = blvSequencerPlay;
	}
	UFUNCTION(BlueprintCallable)
	bool GetLVSequencerStat() {
		return lvSequencerPlay;
	}
	UFUNCTION(BlueprintCallable)
	void ChangeDancing(bool bdance) {
		dancing = bdance;
	}
	bool rideHorse{};
	UPROPERTY(EditAnywhere, Category = "Skill Status")
	bool dancing{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input Status")
	bool F_KeyPressed{};
	bool isAlive{true};
	UPROPERTY(EditAnywhere, Category = "Lv Sequencer")
	bool lvSequencerPlay{true};

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

	float RunningSpeed = 600.0f;
	float WalkingSpeed = 200.0f;
	bool keyinput{};
	class AMain* m_Main;
	int lobby_id;
	int game_id;
	bool bSendInteractionPacket{};
	UPacketExchangeComponent* ControlledPawnPacketExchange = nullptr;
};
