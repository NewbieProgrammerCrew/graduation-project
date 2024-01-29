// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "FuseBox.generated.h"

UCLASS()
class NPC_WORLD_API AFuseBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFuseBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
	int GetIndex() const;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExportInfo")
	int index;
	
	TArray<UStaticMeshComponent*> GetMeshComponent() {
		TArray<UStaticMeshComponent*> MeshComponents;
		GetComponents<UStaticMeshComponent>(MeshComponents);
		return MeshComponents;
	}

	void SetColorId(int c);
	int GetColorId(int c);
	void ChangeBaseColor();
	void ChangeCompleteColor();
	void ChangeActivateEmissiveColor(float value);
	void ActivateFuseBox();
	bool CheckFuseBoxActivate();
	

	void SetOpenedStatus(bool open);
	void GetOpenedStatus(bool& open);
	void OpenFuseBox();

	void StartFillingProgressBar();
	void SetFuseBoxOpenStartPoint(float startpoint);
	void FillProgressBar();
	UFUNCTION(BlueprintCallable)
	void StopFillingProgressBar();
	UFUNCTION(BlueprintCallable)
	float GetFuseBoxCurrentProgress();
private:
	void ProcessCustomEvent(FName FunctionName);
private:
	int color_id;
	bool fuseBoxOpen{};
	bool complete{};
	float startPoint{};
	float CurrentProgressBarValue{};
	bool changed_complted_Color{};
	FTimerHandle ProgressBarTimerHandle;
};
