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
	void UpdateFuseBoxProgressStatus(bool status);
	bool GetFuseBoxProgressStatus();
private:
	int color_id;
	bool complete;
	bool changed_complted_Color;
};
