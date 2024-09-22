// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetworkingThread.h"
#include "GameFramework/Actor.h"
#include "MyGameInstance.h"
#include "Camera/CameraActor.h"
#include "Main.generated.h"

UCLASS()
class NPC_WORLD_API AMain : public AActor
{
	GENERATED_BODY()

public:
	AMain();
	UMyGameInstance* GameInstance = nullptr;
	UFUNCTION(BlueprintCallable)
	void SendMapLoadedPacket();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;
	void ChangeCamera_EscLocCamera();
	void Init();
	class FSocketThread* Network;
	bool init_finish{};
private:
	bool LoadedMap{};
	class ACh_PlayerController* localPlayerController{};
	AActor* cameraActor{};
};
