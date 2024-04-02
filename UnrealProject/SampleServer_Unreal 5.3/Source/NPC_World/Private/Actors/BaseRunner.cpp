// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseRunner.h"
#include "Actors/JellyTemp.h"
#include "Actors/BaseChaser.h"
#include "Animation/AnimInstance.h" 
#include "Animation/AnimMontage.h"
#include "Actors/Bomb.h"
#include "Throwable.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"


// Sets default values
ABaseRunner::ABaseRunner()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_Bomb = nullptr;
	CannonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CannonMesh"));
	CannonMesh->SetupAttachment(GetMesh(), TEXT("WeaponStore_Socket"));

	BombStoreArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("BombStoreArrowComponent"));
	BombStoreArrowComponent->SetupAttachment(CannonMesh);
	BombShootArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("BombShootArrowComponent"));
	BombShootArrowComponent->SetupAttachment(CannonMesh);
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
	AnimInstance = GetMesh()->GetAnimInstance();
}

void ABaseRunner::PlayAttackMontage(UAnimMontage* AttackMontage, FName StartSectionName)
{
	if(m_Bomb && !bshoot) {
		bshoot = true;
		if (AttackMontage) {
			PlayAnimMontage(AttackMontage, 1.0f, StartSectionName);
			Fire();
		}
	}
}

void ABaseRunner::DestroyBomb()
{
	bshoot = false;
	StopAimEvent();
	if (m_Bomb) {
		m_Bomb->Destroy();
		m_Bomb = nullptr;
	}
}

void ABaseRunner::Fire()
{
	ProcessCustomEvent(this, FName("FireEmitter"));
}

void ABaseRunner::Fire(FVector cannonfrontloc, FVector dir)
{
	if (m_Bomb) {
		packetExchange = Cast<UPacketExchangeComponent>
			(GetComponentByClass(UPacketExchangeComponent::StaticClass())); 
		if (!packetExchange) return;
		
		packetExchange->SendCannonFirePacket(cannonfrontloc, dir);

		/*AActor* HitActor = Hit.GetActor();
		AJelly* jelly = Cast<AJelly>(HitActor);
		if (jelly) {
			JellyManager->LookAtPlayer(this, jelly->GetIndex());
			JellyManager->SendExplosionPacket(jelly->GetIndex());
		}*/

		////술래 공격
		//ABaseChaser* chaser = Cast<ABaseChaser>(HitActor);
		//if (chaser) {
		//	ProcessCustomEvent(this, FName("HitChaserEvent"));
		//}
	}
}

// Called every frame
void ABaseRunner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseRunner::Attack()
{
	ProcessCustomEvent(this, FName("AttackEvent"));
}

void ABaseRunner::ShootCannon(FVector pos, FVector dir)
{
	PlayMontage(BombMontage, "Shoot");
	if (m_Bomb) {
		m_Bomb->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		m_Bomb->Fire(pos, dir, 30);
	}
}

void ABaseRunner::DecreaseBomb()
{
	ProcessCustomEvent(this, FName("PistolDecreaseEvent"));
}

void ABaseRunner::PlayAimAnim()
{
	ProcessCustomEvent(this, FName("AimEvent"));
	PlayMontage(BombMontage,"Aim");
	if (m_Bomb) {
		const FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, false);
		m_Bomb->AttachToComponent(BombShootArrowComponent, Rules);
		CannonMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Weapon-Socket"));
	}

}

void ABaseRunner::StopAimEvent()
{
	ProcessCustomEvent(this, FName("StopAimEvent"));
	StopMontage(BombMontage,"Aim");
	if (m_Bomb && !m_Bomb->fire) {
		const FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, false);
		m_Bomb->AttachToComponent(BombStoreArrowComponent, Rules);
	}
	CannonMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponStore_Socket"));
}

void ABaseRunner::EquipBomb(ABomb* newBomb)
{
	m_Bomb = newBomb;
	const FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, false);
	m_Bomb->AttachToComponent(BombStoreArrowComponent, Rules);
	
}

void ABaseRunner::PlayEarnBomb()
{
	ProcessCustomEvent(this, FName("PlayEarnItem"));
}

ABomb* ABaseRunner::GetBomb()
{
	return m_Bomb;
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
AFuseBox* ABaseRunner::GetCurrentOpeningFuseBox()
{
	return FuseBox;
}
void ABaseRunner::SetOpenItemBoxStartPoint(float startpoint)
{
	startPoint = startpoint;
}

void ABaseRunner::PlayAimAnimation(UAnimMontage* AimMontage, FName StartSectionName)
{
	if (m_Bomb) {
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

void ABaseRunner::IsOpeningFuseBox(bool& openingbox)
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
	UDataUpdater* localdataUpdater = GetDataUpdater(); 
	if (!ItemBox) {
		//SetOpeningBox(false);
		if (localdataUpdater) {
			localdataUpdater->ClearOpeningBoxData();
		}
		return false;
	}
	bool boxOpened;
	ItemBox->GetBoxStatus(boxOpened);
	if(boxOpened){
		if (localdataUpdater) {
			localdataUpdater->ClearOpeningBoxData();
		}
		ItemBox = nullptr;
		return false;
	}
	if (localdataUpdater) {
		localdataUpdater->SetCurrentOpeningItem(1);
		localdataUpdater->SetCurrentOpeningItemIndex(ItemBox->GetIndex());
	}
	return true;
}
void ABaseRunner::ClearOpeningBoxData()
{
	SetOpeningBox(false);
	ItemBox = nullptr;
}
bool ABaseRunner::UpdateEquipableBombData(FHitResult Hit, AItemBox* itemBox, UDataUpdater* localdataUpdater) 
{
	UStaticMeshComponent* HitStaticMesh = Cast<UStaticMeshComponent>(Hit.GetComponent());
	if (HitStaticMesh && HitStaticMesh->GetName() == "BombBody") {
		ProcessCustomEvent(itemBox, FName("AvailableBomb"));
		localdataUpdater->SetTempItemBoxIndex(itemBox->GetIndex());
		localdataUpdater->SetTempBombType(itemBox->GetBombItem());
		localdataUpdater->SetBombAvailability(true);
		return true;
	}
	else {
		localdataUpdater->SetBombAvailability(false);
		ProcessCustomEvent(itemBox, FName("DisavailableBomb"));
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

bool ABaseRunner::FindItemBoxAndCheckEquipableBomb(FVector CameraLocation, FRotator CameraRotation, float distance) 
{
	FHitResult Hit = PerformLineTrace(CameraLocation, CameraRotation, distance);
	AItemBox* HitItemBox = Cast<AItemBox>(Hit.GetActor());
	AFuseBox* HitFuseBox = Cast<AFuseBox>(Hit.GetActor());
	UDataUpdater* localdataUpdater = GetDataUpdater();
	if (!localdataUpdater) return false;

	if (HitItemBox) {
		SetCurrentItemBox(HitItemBox);
		prevItemBox = HitItemBox;
		bool boxOpened;
		HitItemBox->GetBoxStatus(boxOpened);
		if (boxOpened) {
			ProcessCustomEvent(HitItemBox, FName("DisavailableItemBox"));
			ProcessCustomEvent(this, FName("HideBoxOpeningUI"));
			localdataUpdater->ClearOpeningBoxData();
			ClearOpeningBoxData();
		}
		else {
			ProcessCustomEvent(HitItemBox, FName("AvailableItemBox"));
			ProcessCustomEvent(this, FName("ShowBoxOpeningUI"));
		}
		checkItemBoxAvailable();
		if (UpdateEquipableBombData(Hit, HitItemBox, localdataUpdater)) {
			ProcessCustomEvent(this, FName("ShowBombAcquiredUI"));
			return true;
		}
		else {
			ProcessCustomEvent(this, FName("HideBombAcquiredUI"));
			return false;
		}
	}
	else if (HitFuseBox) {
		ClearOpeningBoxData();
		return FindFuseBoxInViewAndCheckPutFuse(HitFuseBox);

	}
	else {
		ProcessCustomEvent(FuseBox, FName("ChangeInvalidColor"));
		ProcessCustomEvent(prevItemBox, FName("DisavailableItemBox"));
		prevItemBox = nullptr;
		ProcessCustomEvent(this, FName("SendStopInteractionPAcket"));
		ProcessCustomEvent(this, FName("HideBombAcquiredUI"));
		ProcessCustomEvent(this, FName("HideBoxOpeningUI"));
		ProcessCustomEvent(this, FName("HideUI"));
		localdataUpdater->ClearOpeningBoxData();	
		localdataUpdater->SetFuseBoxOpenAndInstall(-1);
		//SetOpeningFuseBox(false);
		return false;
	}
}
void ABaseRunner::ProcessCustomEvent(AActor* actor, FName Name)
{
	if (!actor) return;
	UFunction* CustomEvent = actor->FindFunction(Name);
	if (CustomEvent) {
		actor->ProcessEvent(CustomEvent, nullptr);
	}
}

void ABaseRunner::StopInteraction()
{
	StopFillingProgressBar();
	SetOpeningFuseBox(false);
	SetOpeningBox(false);
}

void ABaseRunner::PlayMontage(UAnimMontage* MontageToPlay, FName startSection)
{
	if (AnimInstance && MontageToPlay) {
		AnimInstance->Montage_Play(MontageToPlay, 1.f);
		AnimInstance->Montage_JumpToSection(startSection, MontageToPlay);
	}
}

void ABaseRunner::StopMontage(UAnimMontage* MontageToStop, FName startSection)
{
	AnimInstance->Montage_Stop(0.25f, MontageToStop);
}

bool ABaseRunner::FindFuseBoxInViewAndCheckPutFuse(AFuseBox* HitFuseBox)
{
	UDataUpdater* localdataUpdater = GetDataUpdater();
		if (!localdataUpdater) return false;
	if (HitFuseBox && IsFacingFuseBox(HitFuseBox)) {
		bool fuseBoxOpen;
		FuseBox = HitFuseBox;
		HitFuseBox->GetOpenedStatus(fuseBoxOpen);

		if (HitFuseBox->CheckFuseBoxActivate()) {
			ProcessCustomEvent(FuseBox, FName("ChangeInvalidColor"));
			ProcessCustomEvent(this, FName("HideUI"));
			localdataUpdater->ClearOpeningBoxData();
			localdataUpdater->SetFuseBoxOpenAndInstall(-1);
		}
		else if (fuseBoxOpen) {
			ProcessCustomEvent(FuseBox, FName("ChangeValidColor"));
			ProcessCustomEvent(this, FName("ShowFuseInstallUI"));
			int idx = HitFuseBox->GetIndex();
			localdataUpdater->SetFuseBoxOpenAndInstall(idx);
		}
		else {
			ProcessCustomEvent(FuseBox, FName("ChangeValidColor"));
			ProcessCustomEvent(this, FName("ShowFuseBoxOpeningUI"));
			int idx = HitFuseBox->GetIndex();
			localdataUpdater->SetFuseBoxOpenAndInstall(-1);
			localdataUpdater->SetCurrentOpeningItem(2);
			localdataUpdater->SetCurrentOpeningItemIndex(idx);
		}
	}
	else {
		ProcessCustomEvent(FuseBox, FName("ChangeInvalidColor"));
		ProcessCustomEvent(this, FName("HideUI"));
		ProcessCustomEvent(this, FName("SendStopInteractionPAcket"));
		localdataUpdater->ClearOpeningBoxData();
		localdataUpdater->SetFuseBoxOpenAndInstall(-1);
		FuseBox = nullptr;
		return false;
	}
	return true;
}



void ABaseRunner::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
