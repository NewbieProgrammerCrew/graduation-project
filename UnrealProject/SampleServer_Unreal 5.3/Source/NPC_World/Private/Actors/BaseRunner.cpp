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
	CurrentBombType = BombType::NoBomb;
	CannonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CannonMesh"));
	CannonMesh->SetupAttachment(GetMesh(), TEXT("WeaponStore_Socket"));

	BombStoreArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("BombStoreArrowComponent"));
	BombStoreArrowComponent->SetupAttachment(CannonMesh);
	BombShootArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("BombShootArrowComponent"));
	BombShootArrowComponent->SetupAttachment(CannonMesh);

	UpdateOpeningItemBoxStatusWidgetEvent = FindFunction("UpdateOpeningItemBoxStatusWidget");
	FireEmitterEvent = FindFunction("FireEmitter");
	AttackEvent = FindFunction("AttackEvent");
	BombDecreaseEvent = FindFunction("BombDecreaseEvent");
	StopAimCustomEvent = FindFunction("StopAimEvent");
	DestroyBombEvent = FindFunction("DestroyBomb");
	PlayEarnItemEvent = FindFunction("PlayEarnItem");
	PlayOpenBoxEvent = FindFunction("PlayOpenBox");
	PlayOpenFuseBoxEvent = FindFunction("PlayOpenFuseBox");
	HideBoxOpeningUIEvent = FindFunction("HideBoxOpeningUI");
	ShowBoxOpeningUIEvent = FindFunction("ShowBoxOpeningUI");
	ShowBombAcquiredUIEvent = FindFunction("ShowBombAcquiredUI");
	HideBombAcquiredUIEvent = FindFunction("HideBombAcquiredUI");
	SendStopInteractionPacketEvent = FindFunction("SendStopInteractionPacket");
	ShowFuseInstallUIEvent = FindFunction("ShowFuseInstallUI");
	ShowFuseBoxOpeningUIEvent = FindFunction("ShowFuseBoxOpeningUI");
	HideUIEvent = FindFunction("HideUI");
	SkillEvent = FindFunction(FName("SkillEvent"));
	SelectedSkillEvent = FindFunction(FName("SelectedSkillEvent"));
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
	if(CurrentBombType != BombType::NoBomb) {
		if (AttackMontage) {
			PlayAnimMontage(AttackMontage, 1.0f, StartSectionName);
			Fire();
			Aiming = false;
			StopAimEvent();
		}
	}
}

void ABaseRunner::Fire()
{
	ProcessEvent(FireEmitterEvent, nullptr);
}

void ABaseRunner::Fire(FVector cannonfrontloc, FVector dir)
{
	if (CurrentBombType != BombType::NoBomb) {
		packetExchange = Cast<UPacketExchangeComponent>
			(GetComponentByClass(UPacketExchangeComponent::StaticClass())); 
		if (!packetExchange) return;
		
		packetExchange->SendCannonFirePacket(cannonfrontloc, dir);
	}
}

// Called every frame
void ABaseRunner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseRunner::Attack()
{
	ProcessEvent(AttackEvent, nullptr);
}

void ABaseRunner::ActivateSkill()
{
	if (SkillEvent) {
		ProcessEvent(SkillEvent, nullptr);
	}
}


void ABaseRunner::ShootCannon(FVector pos, FVector dir)
{
	if (CurrentBombType != BombType::NoBomb) {
		PlayMontage(BombMontage, "Shoot");
		UClass* BP_StunBombClass = nullptr;
		UClass* BP_ExplosiveBombClass = nullptr;
		UClass* BP_InkBombClass = nullptr;
		ABomb* bomb = nullptr;
		switch (CurrentBombType)
		{
		case BombType::Explosion: {
			BP_ExplosiveBombClass = LoadClass<ABomb>(nullptr, 
									TEXT("Blueprint'/Game/Blueprints/MyActor/BP_ExplosiveBomb.BP_ExplosiveBomb_C'"));
			bomb = GetWorld()->SpawnActor<ABomb>(BP_ExplosiveBombClass);
			bomb->SetActorLocation(BombShootArrowComponent->GetComponentLocation());
			break;
		}
		case BombType::Stun: {
			BP_StunBombClass = LoadClass<ABomb>(nullptr,
							   TEXT("Blueprint'/Game/Blueprints/MyActor/BP_StunBomb.BP_StunBomb_C'"));
			bomb = GetWorld()->SpawnActor<ABomb>(BP_StunBombClass);
			bomb->SetActorLocation(BombShootArrowComponent->GetComponentLocation());
			break;
		}
		case BombType::Blind: {
			BP_InkBombClass = LoadClass<ABomb>(nullptr,
							  TEXT("Blueprint'/Game/Blueprints/MyActor/BP_InkBomb.BP_InkBomb_C'"));
			bomb = GetWorld()->SpawnActor<ABomb>(BP_InkBombClass);
			bomb->SetActorLocation(BombShootArrowComponent->GetComponentLocation());
			break;
		}
		default:
			bomb = nullptr;
			break;
		}
		if (IsValid(bomb)) {
			bomb->SetType(CurrentBombType);
			bomb->Fire(pos, dir, 50);
			CurrentBombType = BombType::NoBomb;
		}
		
	}
}

void ABaseRunner::DecreaseBomb()
{
	ProcessEvent(BombDecreaseEvent, nullptr);
}

void ABaseRunner::PlayAimAnim()
{
	if (CurrentBombType != BombType::NoBomb) {
		Aiming = true;
		PlayMontage(BombMontage, "Aim");
		CannonMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Weapon-Socket"));
	}
	else {
		Aiming = false;
		StopAimEvent();
	}

}

void ABaseRunner::StopAimEvent()
{
	Aiming = false;
	ProcessEvent(StopAimCustomEvent, nullptr);
	StopMontage(BombMontage,"Aim");	
	CannonMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponStore_Socket"));
}

void ABaseRunner::EquipBomb(BombType bombType)
{
	CurrentBombType = bombType;
}

void ABaseRunner::PlayEarnBomb()
{
	ProcessEvent(PlayEarnItemEvent, nullptr);
}

bool ABaseRunner::GetAimStatus()
{
	UDataUpdater* localdataUpdater = GetDataUpdater();
	if (!localdataUpdater) return false;
	return localdataUpdater->hasBomb() && localdataUpdater->GetAimStatus();
}

void ABaseRunner::CallBoxOpenAnimEvent()
{
	//ProcessEvent(PlayOpenBoxEvent, nullptr);
	SetOpeningBox(true);
}
void ABaseRunner::CallFuseBoxOpenAnimEvent()
{
	ProcessEvent(PlayOpenFuseBoxEvent, nullptr);
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
	ProcessEvent(UpdateOpeningItemBoxStatusWidgetEvent,nullptr);
	if (CurrentProgressBarValue >= MaxProgressBarValue) {
		StopFillingProgressBar();
	}
}
void ABaseRunner::SetGameResult(bool win)
{
	bWin = win;
}
void ABaseRunner::GetGameResult(bool& win)
{
	win = bWin;
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
	UDataUpdater* localdataUpdater = GetDataUpdater();
	if (CurrentBombType!=BombType::NoBomb) {
		if (AimMontage){ 
			PlayAnimMontage(AimMontage, 1.0f, StartSectionName);
		}
	}
}
void ABaseRunner::StopPlayAimAnimation(UAnimMontage* AimMontage, FName StartSectionName)
{
	if (AimMontage) {
		StopAnimMontage(AimMontage);
	}
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
			ProcessEvent(HideBoxOpeningUIEvent, nullptr);
			localdataUpdater->ClearOpeningBoxData();
			ClearOpeningBoxData();
		}
		else {
			ProcessCustomEvent(HitItemBox, FName("AvailableItemBox"));
			ProcessEvent(ShowBoxOpeningUIEvent, nullptr);
		}
		checkItemBoxAvailable();
		if (UpdateEquipableBombData(Hit, HitItemBox, localdataUpdater)) {
			ProcessEvent(ShowBombAcquiredUIEvent, nullptr);
			return true;
		}
		else {
			ProcessEvent(HideBombAcquiredUIEvent, nullptr);
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
		ProcessEvent(SendStopInteractionPacketEvent, nullptr);
		ProcessEvent(HideBombAcquiredUIEvent, nullptr);
		ProcessEvent(HideBoxOpeningUIEvent, nullptr);
		ProcessEvent(HideUIEvent, nullptr);
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
			FuseBox->ChangeInvalidColor();
			ProcessEvent(HideUIEvent, nullptr);
			localdataUpdater->ClearOpeningBoxData();
			localdataUpdater->SetFuseBoxOpenAndInstall(-1);
		}
		else if (fuseBoxOpen) {
			FuseBox->ChangeValidColor();
			ProcessEvent(ShowFuseInstallUIEvent, nullptr);
			int idx = HitFuseBox->GetIndex();
			localdataUpdater->SetFuseBoxOpenAndInstall(idx);
		}
		else {
			FuseBox->ChangeValidColor();
			ProcessEvent(ShowFuseBoxOpeningUIEvent, nullptr);
			int idx = HitFuseBox->GetIndex();
			localdataUpdater->SetFuseBoxOpenAndInstall(-1);
			localdataUpdater->SetCurrentOpeningItem(2);
			localdataUpdater->SetCurrentOpeningItemIndex(idx);
		}
	}
	else {
		ProcessCustomEvent(FuseBox, FName("ChangeInvalidColor"));
		ProcessEvent(HideUIEvent, nullptr);
		ProcessEvent(SendStopInteractionPacketEvent, nullptr);
		localdataUpdater->ClearOpeningBoxData();
		localdataUpdater->SetFuseBoxOpenAndInstall(-1);
		FuseBox = nullptr;
		return false;
	}
	return true;
}

int ABaseRunner::GetCurrentBombType()
{
	return int(CurrentBombType);
}



void ABaseRunner::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseRunner::CallSelectedSkillEvent()
{
	if (SelectedSkillEvent) {
		ProcessEvent(SelectedSkillEvent, nullptr);
	}
}
