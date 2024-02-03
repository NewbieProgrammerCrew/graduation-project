// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyMapEditorController.generated.h"

UCLASS()
class NPC_WORLD_API AMyMapEditorController : public APlayerController
{
	GENERATED_BODY()
private:
	AMyMapEditorController();
	class AMapEditorMode* MapEditorMode;
	bool bLeftMousePressed;
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	void HandleLeftClick();
	void HandleLeftClickReleased();
	void AdjustHeight(float DeltaTime);
};
