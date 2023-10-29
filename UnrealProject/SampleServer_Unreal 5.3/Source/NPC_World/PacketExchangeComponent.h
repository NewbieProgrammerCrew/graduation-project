// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../../../Server/ServerTest/ServerTest/protocol.h"
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

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable, Category = "SendPacket")
	void SendHittedPacket();
	UFUNCTION(BlueprintCallable, Category = "SendPacket")
	void SendGetItemPacket(int item_id);

    class FSocketThread* Network;
    class AMyPlayerController* _Controller;
};
