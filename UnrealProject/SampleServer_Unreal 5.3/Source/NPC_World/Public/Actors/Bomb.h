// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../../../../Server/Single Thread ASIO/source/Protocol.h"
#include "Bomb.generated.h"

UCLASS()
class NPC_WORLD_API ABomb : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABomb();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void Fire(FVector initPos, FVector dir, float speed);
	void parabolicTimer();
	void ExplodeBomb();
	void CalculateVelocity(float speed, FVector dir);
	FVector parabolicMotion(const FVector& initialPosition, double time);

	void SetType(BombType type);
	int GetType();
	int bombIndex{};
	bool fire{};
private:
	UFunction* ExplosionEvent;
	int m_Type;
	FTimerHandle TimerHandle_CalculateParabolic;
	FVector bombLocation;
	FVector initialVelocity;
	FVector acceleration;
	float floor_z = 51.f;
	float sec{};
};
