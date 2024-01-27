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
				ProcessCustomEvent(this, FName("PistolDecreaseEvent"));
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
	ProcessCustomEvent(this, FName("SendIdlePacket"));
	if (m_gun) {
		m_gun->Destroy();
		m_gun = nullptr;
	}
}

void ABaseRunner::Fire()
{
	ProcessCustomEvent(this, FName("FireEmitter"));
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
	ProcessCustomEvent(this, FName("AttackEvent"));
}

ABaseGun* ABaseRunner::GetGun()
{
	return m_gun;
}

void ABaseRunner::CallAimAnimEvent()
{
	ProcessCustomEvent(this, FName("AimAnimEvent"));
}

void ABaseRunner::CallStopAimAnimEvent()
{
	ProcessCustomEvent(this, FName("StopAimAnimEvent"));
}

void ABaseRunner::CallBoxOpenAnimEvent()
{
	ProcessCustomEvent(this, FName("PlayOpenBox"));
}
void ABaseRunner::CallFuseBoxOpenAnimEvent()
{
	ProcessCustomEvent(this, FName("PlayOpenFuseBox"));
}
void ABaseRunner::StartFillingProgressBar()
{
	CurrentProgressBarValue = startPoint;
	GetWorld()->GetTimerManager().SetTimer(ProgressBarTimerHandle, this, &ABaseRunner::FillProgressBar, 0.03f, true);
}

void ABaseRunner::StopFillingProgressBar()
{
	GetWorld()->GetTimerManager().ClearTimer(ProgressBarTimerHandle);
	CurrentProgressBarValue = 0.0f;
}

void ABaseRunner::FillProgressBar()
{
	float MaxProgressBarValue = 1;
	CurrentProgressBarValue += 0.01f;
	ProcessCustomEvent(this, FName("UpdateOpeningItemBoxStatusWidget"));
	if (CurrentProgressBarValue >= MaxProgressBarValue) {
		StopFillingProgressBar();
	}
}
void ABaseRunner::SetOpenItemBoxStartPoint(float startpoint)
{
	startPoint = startpoint;
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
void ABaseRunner::SetOpeningBox(bool openingbox)
{
	bOpeningBox = openingbox;
}
void ABaseRunner::GetOpeningBox(bool& openingbox)
{
	openingbox = bOpeningBox;
}

void ABaseRunner::SetOpeningFuseBox(bool openingbox)
{
	bOpeningFuseBox = openingbox;
}

void ABaseRunner::GetOpeningFuseBox(bool& openingbox)
{
	openingbox = bOpeningFuseBox;
}
void ABaseRunner::SetCurrentItemBox(AItemBox* itembox)
{
	ItemBox = itembox;
}
float ABaseRunner::GetCurrentOpeningItemBoxProgress()
{
	return CurrentProgressBarValue;
}
FHitResult ABaseRunner::PerformLineTrace(FVector CameraLocation, FRotator CameraRotation, float distance)
{
	FVector ShotDirection = CameraRotation.Vector();
	FVector TraceEnd = CameraLocation + (ShotDirection * distance);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;

	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, CameraLocation, TraceEnd, ECC_Visibility, QueryParams);
	return Hit;
}
bool ABaseRunner::checkItemBoxAvailable()
{
	UDataUpdater* local_DataUpdater = Cast<UDataUpdater>(GetComponentByClass(UDataUpdater::StaticClass()));
	if (!ItemBox) {
		//SetOpeningBox(false);
		if (local_DataUpdater) {
			local_DataUpdater->ClearOpeningBoxData();
		}
		return false;
	}
	bool boxOpened;
	ItemBox->GetBoxStatus(boxOpened);
	if(boxOpened){
		if (local_DataUpdater) {
			local_DataUpdater->ClearOpeningBoxData();
		}
		ItemBox = nullptr;
		return false;
	}
	if (local_DataUpdater) {
		local_DataUpdater->SetCurrentOpeningItem(1);
		local_DataUpdater->SetCurrentOpeningItemIndex(ItemBox->GetIndex());
	}
	return true;
}
void ABaseRunner::ClearOpeningBoxData()
{
	SetOpeningBox(false);
	ItemBox = nullptr;
}
bool ABaseRunner::UpdateEquipableGunData(FHitResult Hit, AItemBox* itemBox, UDataUpdater* dataUpdater) 
{
	UStaticMeshComponent* HitStaticMesh = Cast<UStaticMeshComponent>(Hit.GetComponent());
	if (HitStaticMesh && HitStaticMesh->GetName() == "Pistol") {
		ProcessCustomEvent(itemBox, FName("AvailableGun"));
		dataUpdater->SetTempItemBoxIndex(itemBox->GetIndex());
		dataUpdater->SetTempGunType(itemBox->GetGunItem());
		dataUpdater->SetGunAvailability(true);
		return true;
	}
	else {
		ProcessCustomEvent(itemBox, FName("DisavailableGun"));
		return false;
	}
}
bool ABaseRunner::IsFacingFuseBox(AFuseBox* FacingFuseBox)
{
	if (!FacingFuseBox) return false;

	FVector PlayerForwardVector = GetActorForwardVector();
	FVector FuseBoxRightVector = FacingFuseBox->GetActorRightVector();
	PlayerForwardVector.Z = 0;
	FuseBoxRightVector.Z = 0;

	PlayerForwardVector.Normalize();
	FuseBoxRightVector.Normalize();

	float DotProduct = FVector::DotProduct(PlayerForwardVector, FuseBoxRightVector);

	return FMath::Abs(DotProduct) > 0.98f; 
}

bool ABaseRunner::FindItemBoxAndCheckEquipableGun(FVector CameraLocation, FRotator CameraRotation, float distance) 
{
	FHitResult Hit = PerformLineTrace(CameraLocation, CameraRotation, distance);
	AItemBox* HitItemBox = Cast<AItemBox>(Hit.GetActor());
	AFuseBox* HitFuseBox = Cast<AFuseBox>(Hit.GetActor());
	UDataUpdater* local_DataUpdater = Cast<UDataUpdater>(GetComponentByClass(UDataUpdater::StaticClass()));
	if (!local_DataUpdater) return false;

	if (HitItemBox) {
		SetCurrentItemBox(HitItemBox);
		bool boxOpened;
		HitItemBox->GetBoxStatus(boxOpened);
		if (boxOpened) {
			ProcessCustomEvent(this, FName("HideBoxOpeningUI"));
			local_DataUpdater->ClearOpeningBoxData();
			ClearOpeningBoxData();
		}
		else {
			ProcessCustomEvent(this, FName("ShowBoxOpeningUI"));
		}
		checkItemBoxAvailable();
		if (UpdateEquipableGunData(Hit, HitItemBox, local_DataUpdater)) {
			ProcessCustomEvent(this, FName("ShowGunAcquiredUI"));
			return true;
		}
		else {
			ProcessCustomEvent(this, FName("HideGunAcquiredUI"));
			return false;
		}
	}
	else if (HitFuseBox) {
		ClearOpeningBoxData();
		return FindFuseBoxInViewAndCheckPutFuse(HitFuseBox);

	}
	else {
		ProcessCustomEvent(this, FName("HideGunAcquiredUI"));
		ProcessCustomEvent(this, FName("HideBoxOpeningUI"));
		ProcessCustomEvent(this, FName("HideUI"));
		local_DataUpdater->ClearOpeningBoxData();	
		local_DataUpdater->SetFuseBoxOpenAndInstall(-1);
		SetOpeningFuseBox(false);
		return false;
	}
}
void ABaseRunner::ProcessCustomEvent(AActor* actor, FName Name)
{
	UFunction* CustomEvent = actor->FindFunction(Name);
	if (CustomEvent) {
		actor->ProcessEvent(CustomEvent, nullptr);
	}
}

void ABaseRunner::StopInteraction()
{
	StopFillingProgressBar();
	if (FuseBox)
		FuseBox->StopFillingProgressBar();
}

bool ABaseRunner::FindFuseBoxInViewAndCheckPutFuse(AFuseBox* HitFuseBox)
{
	UDataUpdater* local_DataUpdater = Cast<UDataUpdater>(GetComponentByClass(UDataUpdater::StaticClass()));
	if (!local_DataUpdater) return false;
	if (HitFuseBox && IsFacingFuseBox(HitFuseBox)) {
		bool fuseBoxOpen;
		FuseBox = HitFuseBox;
		HitFuseBox->GetOpenedStatus(fuseBoxOpen);

		if (HitFuseBox->CheckFuseBoxActivate()) {
			ProcessCustomEvent(this, FName("HideUI"));
			local_DataUpdater->ClearOpeningBoxData();
			local_DataUpdater->SetFuseBoxOpenAndInstall(-1);
			SetOpeningFuseBox(false);
		}
		else if (fuseBoxOpen) {
			ProcessCustomEvent(this, FName("ShowFuseInstallUI"));
			int idx = HitFuseBox->GetIndex();
			local_DataUpdater->SetFuseBoxOpenAndInstall(idx);
			//SetOpeningFuseBox(false);
		}
		else {
			ProcessCustomEvent(this, FName("ShowFuseBoxOpeningUI"));
			int idx = HitFuseBox->GetIndex();
			local_DataUpdater->SetFuseBoxOpenAndInstall(-1);
			local_DataUpdater->SetCurrentOpeningItem(2);
			local_DataUpdater->SetCurrentOpeningItemIndex(idx);
		}
	}
	else if (HitFuseBox) {
		//SetOpeningFuseBox(false);
		ProcessCustomEvent(this, FName("HideUI"));
		local_DataUpdater->ClearOpeningBoxData();
		local_DataUpdater->SetFuseBoxOpenAndInstall(-1);
		return false;
	}
	else {
		FuseBox = nullptr;
	}
	return true;
}


void ABaseRunner::Fire(FVector CameraLocation, FRotator CameraRotation, float distance,
	UParticleSystem* ExplosionEffect, UParticleSystem* StunEffect, UParticleSystem* InkEffect, FVector ParticleScale)
{
	if (m_gun) {
		UParticleSystem* ImpactEffect = nullptr;
		FHitResult Hit = PerformLineTrace(CameraLocation, CameraRotation, distance);
		if (Hit.GetActor()) {
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

void ABaseRunner::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
