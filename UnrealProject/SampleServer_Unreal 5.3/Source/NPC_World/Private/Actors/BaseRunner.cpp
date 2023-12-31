// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseRunner.h"
#include "Actors/JellyTemp.h"
#include "Animation/AnimInstance.h" 
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

// Sets default values
ABaseRunner::ABaseRunner()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_gun = nullptr;
}

// Called when the game starts or when spawned
void ABaseRunner::BeginPlay()
{
	Super::BeginPlay();
	UWorld* world = GetWorld();
	if (!world) {
		return;
	}
	AActor* actor = UGameplayStatics::GetActorOfClass(world, AJellyManager::StaticClass());
	if (actor == nullptr) {
		return;
	}
	actor->GetWorld();
	JellyManager = Cast<AJellyManager>(actor);
}

void ABaseRunner::PlayAttackMontage(UAnimMontage* AttackMontage, FName StartSectionName)
{
	if(m_gun && !bshoot) {
		bshoot = true;
		if (AttackMontage) {
			PlayAnimMontage(AttackMontage, 1.0f, StartSectionName);
			Fire();
			int bullets = m_gun->GetBulletCount();
			if (bullets <= 0) {
				UFunction* PistolUpdateWidgetEvent = FindFunction(FName("PistolDecreaseEvent"));
				if (PistolUpdateWidgetEvent) {
					ProcessEvent(PistolUpdateWidgetEvent, nullptr);
				}
				FTimerHandle UnusedHandle;
				GetWorldTimerManager().SetTimer(UnusedHandle, this, &ABaseRunner::DestroyGun, 0.7f, false);
			}
		}
	}
}

void ABaseRunner::DestroyGun()
{
	bshoot = false;
	CallStopAimAnimEvent();
	UFunction* SendIdlePacketEvent = FindFunction(FName("SendIdlePacket"));
	if (SendIdlePacketEvent) {
		ProcessEvent(SendIdlePacketEvent, nullptr);
	}

	if (m_gun) {
		m_gun->Destroy();
		m_gun = nullptr;
	}
}

void ABaseRunner::Fire()
{
	UFunction* FireEmitterEvent = m_gun->FindFunction(FName("FireEmitter"));
	if (FireEmitterEvent) {
		m_gun->ProcessEvent(FireEmitterEvent, nullptr);
	}
	m_gun->Fire();

}

// Called every frame
void ABaseRunner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseRunner::EquipGun(ABaseGun* newGun)
{
	if (m_gun) {
		m_gun->Destroy();
		m_gun = nullptr;
	}
	if (newGun) {
		m_gun = newGun;
		newGun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Weapon-Socket"));
	}
}
void ABaseRunner::Attack()
{
	UFunction* AttackEvent = FindFunction(FName("AttackEvent"));
	if (AttackEvent) {
		ProcessEvent(AttackEvent, nullptr);
	}
}

ABaseGun* ABaseRunner::GetGun()
{
	return m_gun;
}

void ABaseRunner::CallAimAnimEvent()
{
	UFunction* AimAnimEvent = FindFunction(FName("AimAnimEvent"));
	if (AimAnimEvent) {
		ProcessEvent(AimAnimEvent, nullptr);
	}
}

void ABaseRunner::CallStopAimAnimEvent()
{
	UFunction* StopAimAnimEvent = FindFunction(FName("StopAimAnimEvent"));
	if (StopAimAnimEvent) {
		ProcessEvent(StopAimAnimEvent, nullptr);
	}
}

void ABaseRunner::PlayAimAnimation(UAnimMontage* AimMontage, FName StartSectionName)
{
	if (m_gun) {
		if (AimMontage){ 
			PlayAnimMontage(AimMontage, 1.0f, StartSectionName);
		}
	}
}
void ABaseRunner::StopPlayAimAnimation(UAnimMontage* AimMontage, FName StartSectionName)
{
	if (AimMontage)
		StopAnimMontage(AimMontage);
}
void ABaseRunner::Fire(FVector CameraLocation, FRotator CameraRotation, float distance, 
	UParticleSystem* ExplosionEffect, UParticleSystem* StunEffect, UParticleSystem* InkEffect, FVector ParticleScale)
{
	if (m_gun) {
		UParticleSystem* ImpactEffect = nullptr;
		FVector ShotDirection = CameraRotation.Vector();
		FVector TraceEnd = CameraLocation + (ShotDirection * distance);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, CameraLocation, TraceEnd, ECC_Visibility, QueryParams)) {
			switch (m_gun->GetType()) {
			case 0:
				if (StunEffect) {
					ImpactEffect = StunEffect;
				}
				break;
			case 1:
				if (ExplosionEffect) {
					ImpactEffect = ExplosionEffect;
				}
				break;
			case 2:
				if (InkEffect) {
					ImpactEffect = InkEffect;
				}
				break;
			default:
				break;
			}

			if (ImpactEffect) {
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactEffect,
					Hit.ImpactPoint,
					Hit.ImpactNormal.Rotation(),
					ParticleScale
				);
			}
			AActor* HitActor = Hit.GetActor();
			AJelly* jelly = Cast<AJelly>(HitActor);
			if (jelly) {
				JellyManager->SendExplosionPacket(jelly->GetIndex());
			}
		}
	}
}

// Called to bind functionality to input
void ABaseRunner::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

