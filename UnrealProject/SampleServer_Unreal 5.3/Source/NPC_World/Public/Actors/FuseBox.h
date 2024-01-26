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
	// Called every frame
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
	void ActivateFuseBox();
	bool CheckFuseBoxActivate();
	
	float GetFuseBoxCurrentProgress();
	void SetFuseBoxProgress(float progress);

	void SetOpenedStatus(bool open);
	void GetOpenedStatus(bool& open);
	void OpenFuseBox();
private:
	int color_id;
	bool fuseBoxOpen{};
	bool complete{};
	float progress{};
	bool changed_complted_Color{};
};
