// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseGun.generated.h"

UENUM(BlueprintType)
enum class EGunType : uint8
{
	StunGun UMETA(DisplayName = "Stun Gun"),   // ±âÀýÃÑ
	ExplosiveGun UMETA(DisplayName = "Explosive Gun"), // Æø¹ßÃÑ
	InkGun UMETA(DisplayName = "Ink Gun")      // ¸Ô¹°ÃÑ
	// Ãß°¡ Å¸ÀÔ Á¤ÀÇ °¡´É
};
UCLASS()
class NPC_WORLD_API ABaseGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseGun();
	ABaseGun(int n);
	virtual void Fire();
	virtual int GetBulletCount();
	virtual void UpdateBulletCount(int n);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* GunMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	int bullets;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
};
