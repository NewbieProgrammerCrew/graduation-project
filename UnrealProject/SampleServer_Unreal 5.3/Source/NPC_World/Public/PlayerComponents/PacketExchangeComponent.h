// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../../../../Server/Single Thread ASIO/source/Protocol.h" 
#include "../../../../../Server/Lobby Server/protocol.h"
#include "Components/ActorComponent.h"
#include "PacketExchangeComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NPC_WORLD_API UPacketExchangeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPacketExchangeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:	
	// Called every frame
	UFUNCTION(BlueprintCallable, Category = "SendPacket")
	void SendAttackPacket();
	UFUNCTION(BlueprintCallable, Category = "SendPacket")
	void SendCannonFirePacket(FVector cannonfrontloc, FVector dir);

	void SendInteractionPacket();
	UFUNCTION(BlueprintCallable, Category = "SendPacket") 
	void SendEscapePacket();
	UFUNCTION(BlueprintCallable, Category = "SendPacket")
	void SendInteractionEndPacket();
	UFUNCTION(BlueprintCallable, Category = "SendPacket")
	void SendGetItemPacket(int item_id);
	UFUNCTION(BlueprintCallable, Category = "SendPacket")
	void SendGetBombPacket(int bomb_type, int item_idx);
	UFUNCTION(BlueprintCallable, Category = "SendPacket")
	void SendUsedBombPacket();
	
	UFUNCTION(BlueprintCallable, Category = "SendPacket")
	void SendMovePacket(float PitchValue, bool didYouJump = false);

	UFUNCTION(BlueprintCallable, Category = "SendPacket")
	void SendAimPacket();
	UFUNCTION(BlueprintCallable, Category = "SendPacket")
	void SendIdlePacket();
	UFUNCTION(BlueprintCallable, Category = "SendPacket")
	void SendUseSkillPacket(int skilltype);
	UFUNCTION(BlueprintCallable, Category = "Bomb") 
	void CheckEquipmentBomb();
	
private:
	void CalculateMovement();
	void CalculateSpeed();
	void CalculateFalling();

private:
    class FSocketThread* Network;
    class ACh_PlayerController* _Controller;
	class UDataUpdater* DataUpdater = nullptr;
	bool didjump;
	bool sendPressF{};
};
