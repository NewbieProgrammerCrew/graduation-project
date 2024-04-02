// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Bomb.h"

// Sets default values
ABomb::ABomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABomb::BeginPlay()
{
	Super::BeginPlay();
    acceleration = { 0, 0, -9.8 };
}

// Called every frame
void ABomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABomb::Fire(FVector initPos, FVector dir, float speed)
{
    UWorld* world = GetWorld();
    CalculateVelocity(speed, dir);
    bombLocation = initPos;
    const float parabolicTime = 0.001f;
    fire = true;
    
    UFunction* CustomEvent = FindFunction("ColOn");
    if (CustomEvent) {
        ProcessEvent(CustomEvent, nullptr);
    }

    if(world)
        world->GetTimerManager().SetTimer(TimerHandle_CalculateParabolic, this, 
                                &ABomb::parabolicTimer, parabolicTime, true);
}

void ABomb::CalculateVelocity(float speed, FVector direction)
{
    direction.Normalize();
    double vx = speed * direction.X;
    double vy = speed * direction.Y;
    double vz = speed * direction.Z;

    initialVelocity = { vx, vy, vz };
}
void ABomb::parabolicTimer() {
    sec += 0.01f;
    FVector newLoc = parabolicMotion(bombLocation, sec);
    SetActorLocation(newLoc, true);
    bombLocation = newLoc;
}
FVector ABomb::parabolicMotion(const FVector& initialPosition, double time) 
{
    FVector halfAccel = acceleration * 0.5;
    FVector displacement = initialVelocity * time + halfAccel * (time * time);
    return initialPosition + displacement;
}

int ABomb::GetType()
{
	return m_Type;
}


void ABomb::SetType(EBombType type)
{
	m_Type = type;
}

