// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBox.generated.h"

UCLASS()
class NPC_WORLD_API AItemBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
public:	

	UFUNCTION(BlueprintCallable)
	int GetIndex() const;
	UFUNCTION(BlueprintCallable)
	int GetBombItem();
	void SetBombItem(int Bombtype);
	UFUNCTION(BlueprintCallable)
	void GetBoxStatus(bool& boxOpen);
	UFUNCTION(BlueprintCallable)
	void SetBoxStatus(bool boxOpen);
	
	void HideBombItem();
	void ShowBombItem();
private:
	TArray<UStaticMeshComponent*> GetMeshComponent() {
		TArray<UStaticMeshComponent*> MeshComponents;
		GetComponents<UStaticMeshComponent>(MeshComponents);
		return MeshComponents;
	}

public:
	UPROPERTY(EditAnywhere)
	TArray<UStaticMesh*> Bomb;
	UPROPERTY(EditAnywhere)
	TArray<UMaterialInstance*> materialsBomb;
	UPROPERTY(EditAnywhere)
	int idx{};
	int m_Bombtype{};
	bool hasBomb{};
private:
	bool Openend{};
};
