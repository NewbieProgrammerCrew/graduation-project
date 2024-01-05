// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseRunner.h"
#include "Animation/AnimInstance.h" 
#include "Animation/AnimMontage.h"

// Sets default values
ABaseRunner::ABaseRunner()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseRunner::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseRunner::PlayAttackMontage()
{
	//if (aiming && !bshoot)
	if(m_gun && !bshoot) {
		bshoot = true;
		FSoftObjectPath MontagePath(TEXT("/Game/Animation/Haribo/AMTG_Shoot.AMTG_Shoot"));
		UAnimMontage* AttackMontage = Cast<UAnimMontage>(MontagePath.TryLoad());
		if (AttackMontage) {
			FName StartSectionName = "Shoot";
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
	UFunction* StopAimCustomEvent = FindFunction(FName("StopAimAnimEvent"));
	if (StopAimCustomEvent) {
		ProcessEvent(StopAimCustomEvent, nullptr);
	}
	if (m_gun) {
		m_gun->Destroy();
		m_gun = nullptr;
		// call stopanim
		FSoftObjectPath MontagePath(TEXT("/Game/Animation/Haribo/AMTG_Shoot.AMTG_Shoot"));
		UAnimMontage* AttackMontage = Cast<UAnimMontage>(MontagePath.TryLoad());
		if(AttackMontage)
			StopAnimMontage(AttackMontage);
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

ABaseGun* ABaseRunner::GetGun()
{
	return m_gun;
}

void ABaseRunner::Attack()
{
	if (m_gun) {
		PlayAttackMontage();
	}
}

void ABaseRunner::SetAimMode()
{
	if (m_gun) {
		aiming = true;
		FSoftObjectPath MontagePath(TEXT("/Game/Animation/Haribo/AMTG_Shoot.AMTG_Shoot"));
		UAnimMontage* AimMontage = Cast<UAnimMontage>(MontagePath.TryLoad());
		if (AimMontage)
		{
			FName StartSectionName = "Aim";
			PlayAnimMontage(AimMontage, 1.0f, StartSectionName);
		}
	}
}


// Called to bind functionality to input
void ABaseRunner::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

