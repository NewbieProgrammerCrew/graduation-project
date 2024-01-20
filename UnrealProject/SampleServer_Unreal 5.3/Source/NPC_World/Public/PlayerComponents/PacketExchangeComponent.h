// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../../../../../Server/ServerTest/ServerTest/protocol.h"
#include "Components/ActorComponent.h"
#include "PacketExchangeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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
	void SendHittedPacket();
	void SendAttackPacket(int id);
	void SendInteractionPacket();
	void SendInteractionEndPacket();
	UFUNCTION(BlueprintCallable, Category = "SendPacket")
	void SendGetItemPacket(int item_id);
	UFUNCTION(BlueprintCallable, Category = "SendPacket")
	void SendGetPistolPacket(int pistol_type);
	
	UFUNCTION(BlueprintCallable, Category = "SendPacket")
	void SendMovePacket(int speed=-1, bool didYouJump = false);

	UFUNCTION(BlueprintCallable, Category = "SendPacket")
	void SendAimPacket();
	UFUNCTION(BlueprintCallable, Category = "SendPacket")
	void SendIdlePacket();
	
	
private:
	void CalculateMovement();
	void CalculateSpeed();
	void CalculateFalling();

private:
    class FSocketThread* Network;
    class ACh_PlayerController* _Controller;
	class UDataUpdater* DataUpdater = nullptr;
	bool didjump;
};
