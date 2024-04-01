// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bomb.generated.h"

enum EBombType
{
	StunBomb,
	ExplosiveBomb,
	InkBomb
	
};
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
	void Fire(FRotator rot, float speed);
	void parabolicTimer();
	void CalculateVelocity(float speed, FVector2d rot);
	FVector parabolicMotion(const FVector& initialPosition, double time);

	void SetType(EBombType type);
	int GetType();
private:
	int m_Type;
	FTimerHandle TimerHandle_CalculateParabolic;
	FVector initialVelocity;
	FVector acceleration;
	double  M_PI = 3.14159265358979323846;
	float sec{};
};
