// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Manager/FuseBoxManager.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AFuseBoxManager::AFuseBoxManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Network = nullptr;
}

// Called when the game starts or when spawned
void AFuseBoxManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFuseBoxManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!Network && GameInstance->Network) {
		Network = GameInstance->Network;
		if (!GameInstance->Network->_FuseBoxManager) {
			GameInstance->Network->_FuseBoxManager = this;
		}
	}
	SC_FUSE_BOX_ACTIVE_PACKET ActiveFuseBox;
	while (!FuseBox_Active_Queue.empty()) {
		if (FuseBox_Active_Queue.try_pop(ActiveFuseBox)) {
			SetCompleteFuseBox(ActiveFuseBox);
		}
	}
	SC_OPENING_FUSE_BOX_PACKET OpeningFuseBox;
	while (!FuseBox_Opening_Queue.empty()) {
		if (FuseBox_Opening_Queue.try_pop(OpeningFuseBox)) {
			SaveFuseBoxProgressRatio(OpeningFuseBox);
		}
	}
	SC_FUSE_BOX_OPENED_PACKET OpenedFuseBox;
	while (!FuseBox_Opened_Queue.empty()) {
		if (FuseBox_Opened_Queue.try_pop(OpenedFuseBox)) {
			PlayOpenedFuseBoxAnim(OpenedFuseBox);
			
		}
	}
	/*SC_RESET_FUSE_BOX_PACKET ResetFuseBox;
	while (!FuseBox_Reset_Queue.empty()) {
		if (FuseBox_Reset_Queue.try_pop(ResetFuseBox)) {
			ResetFuseBoxStatus(ResetFuseBox);
		}
	}*/
	SC_STOP_OPENING_PACKET StopedFuseBox;
	while (!FuseBox_Stop_Opening_Queue.empty()) {
		if (FuseBox_Stop_Opening_Queue.try_pop(StopedFuseBox)) {
			StopOpeningFuseBox(StopedFuseBox);
		}
	}

}
void AFuseBoxManager::InitFuseBox()
{
	GameInstance = Cast<UMyGameInstance>(GetGameInstance());

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), fuseBoxActor->GetClass(), FuseBoxes);
	FuseBoxes.Sort([&](const AActor& A, const AActor& B) {
		return A.GetName() < B.GetName();
		});
	int idx{};
	for (auto j : FuseBoxes) {
		Cast<AFuseBox>(j)->SetIndex(idx);
		++idx;
	}


	for (const auto& f : FuseBoxes) {
		AFuseBox* fusebox = Cast<AFuseBox>(f);
		fusebox->SetActorEnableCollision(false);
		auto MeshArray = fusebox->GetMeshComponent();
		for (const auto& mesh : MeshArray) {
			mesh->SetVisibility(false);
		}
		UFunction* HiddenEvent = fusebox->FindFunction(FName("HiddenWidgetAndCable"));
		if (HiddenEvent) {
			fusebox->ProcessEvent(HiddenEvent, nullptr);
		}
	}

	TArray<int> colors = GameInstance->GetActivedFuseBoxColorId();
	TArray<int> ActiveIdx = GameInstance->GetActiveFuseBoxIndex();
	if (!(colors.IsEmpty() || ActiveIdx.IsEmpty())) {
		for (int i{}; i < 8; ++i) {
			ActiveFuseBox(ActiveIdx[i]);
			if (ActiveIdx[i] < FuseBoxes.Num()) {
				AFuseBox* fuseBox = Cast<AFuseBox>(FuseBoxes[ActiveIdx[i]]);

				fuseBox->SetColorId(colors[i]);
				fuseBox->ChangeBaseColor();
			}
			else {
				int v = ActiveIdx[i];
				GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Black, FString::Printf(TEXT("FuseBoxManager: out of range! %d"), v));
			}
		}
	}
}
// 0 : fuseBox
// 1 : fuseBox Cover
// 2 : fuse
void AFuseBoxManager::ActiveFuseBox(int idx)
{
	FuseBoxes[idx]->SetActorEnableCollision(true);

	auto MeshArray = Cast<AFuseBox>(FuseBoxes[idx])->GetMeshComponent();
	
	for (auto m : MeshArray) {
		if (m->GetName().Contains("fuse")) {
			m->SetVisibility(false);
		}
		else {
			m->SetVisibility(true);
		}
	}
	UFunction* VisibleEvent = FuseBoxes[idx]->FindFunction(FName("VisibleCableAndWidget"));
	if (VisibleEvent) {
		FuseBoxes[idx]->ProcessEvent(VisibleEvent, nullptr);
	}
}

void AFuseBoxManager::SetCompleteFuseBox(SC_FUSE_BOX_ACTIVE_PACKET packet)
{
	int idx = packet.fuseBoxIndex;
	if (idx < 0 || idx > FuseBoxes.Num() - 1) return;

	Cast<AFuseBox>(FuseBoxes[idx])->ActivateFuseBox();
	auto MeshArray = Cast<AFuseBox>(FuseBoxes[idx])->GetMeshComponent();
	MeshArray[2]->SetVisibility(true);
}

void AFuseBoxManager::SaveFuseBoxProgressRatio(SC_OPENING_FUSE_BOX_PACKET packet)
{
	int idx = packet.index;
	if (idx < 0 || idx > FuseBoxes.Num() - 1) return;
	float startPoint = packet.progress;
	AFuseBox* fuseBox =  Cast<AFuseBox>(FuseBoxes[idx]);
	fuseBox->SetFuseBoxOpenStartPoint(startPoint);
	fuseBox->StartFillingProgressBar();
	fuseBox->ChangeActivateEmissiveColor(5);
}

void AFuseBoxManager::PlayOpenedFuseBoxAnim(SC_FUSE_BOX_OPENED_PACKET packet)
{
	int idx = packet.index;
	if (idx < 0 || idx > FuseBoxes.Num() - 1) return;
	AFuseBox* fuseBox = Cast<AFuseBox>(FuseBoxes[idx]);
	fuseBox->OpenFuseBox();
}

//void AFuseBoxManager::ResetFuseBoxStatus(SC_RESET_FUSE_BOX_PACKET packet)
//{
//	int idx = packet.index;
//	if (idx< 0 || idx > FuseBoxes.Num() - 1) return;
//	AFuseBox* fuseBox = Cast<AFuseBox>(FuseBoxes[idx]);
//	fuseBox->ResetFuseBox();
//}

void AFuseBoxManager::StopOpeningFuseBox(SC_STOP_OPENING_PACKET packet)
{
	int idx = packet.index;
	if (idx < 0 || idx > FuseBoxes.Num() - 1) return;
	AFuseBox* fuseBox = Cast<AFuseBox>(FuseBoxes[idx]);
	fuseBox->StopFillingProgressBar();
	fuseBox->ChangeActivateEmissiveColor(0);
}


void AFuseBoxManager::Set_FuseBox_Active_Queue(SC_FUSE_BOX_ACTIVE_PACKET* packet)
{
	FuseBox_Active_Queue.push(*packet);
}

void AFuseBoxManager::Set_FuseBox_Opened_Queue(SC_FUSE_BOX_OPENED_PACKET* packet)
{
	FuseBox_Opened_Queue.push(*packet);
}
void AFuseBoxManager::Set_Stop_Opening_Queue(SC_STOP_OPENING_PACKET* packet)
{
	FuseBox_Stop_Opening_Queue.push(*packet);
}

AFuseBox* AFuseBoxManager::GetFuseBoxInArraybyIndex(int idx)
{
	if (idx < 0 || idx >= FuseBoxes.Num() || !FuseBoxes[idx]){
		return nullptr;
	}
	AFuseBox* fuseBox = Cast<AFuseBox>(FuseBoxes[idx]);
	return 	fuseBox;
}

void AFuseBoxManager::Set_FuseBox_Opening_Queue(SC_OPENING_FUSE_BOX_PACKET* packet)
{
	FuseBox_Opening_Queue.push(*packet);
}
//
//void AFuseBoxManager::Set_FuseBox_Reset_Queue(SC_RESET_FUSE_BOX_PACKET* packet)
//{
//	FuseBox_Reset_Queue.push(*packet);
//}
//
//
