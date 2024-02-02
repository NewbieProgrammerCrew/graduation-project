// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Actors/FuseBox.h"

// Sets default values
AFuseBox::AFuseBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	index = 0;
	color_id = -1;
	changed_complted_Color = false;
}

// Called when the game starts or when spawned
void AFuseBox::BeginPlay()
{
	Super::BeginPlay();
	
}
// Called every frame
void AFuseBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (complete && !changed_complted_Color) {
		ChangeCompleteColor();
		changed_complted_Color = true;
	}
}
int AFuseBox::GetIndex() const
{
	return index;
}
void AFuseBox::SetColorId(int c)
{
	color_id = c;
}
int AFuseBox::GetColorId(int c)
{
	return color_id;
}
// 4 handprint
// 3 platform
// 2 static mesh
// 1 Fusebox cover
// 0 sm fusebox
void AFuseBox::ChangeBaseColor()
{
	TArray<UStaticMeshComponent*> mesh = GetMeshComponent();
	for (int i{}; i < mesh.Num() - 1; ++i) {
		int materialIdx = 0;
		if (i == 0)
			materialIdx = 1;
		else
			materialIdx = 0;

		UMaterialInterface* Material = mesh[i]->GetMaterial(materialIdx);
		UMaterialInstanceDynamic* MaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
		mesh[i]->SetMaterial(materialIdx, MaterialInstance);
		FLinearColor NewColor = FLinearColor(0.0f, 0.0f, 0.0f);

		switch (color_id) {
		case 0:
			NewColor = FLinearColor(0.43f, 0.0f, 0.08f);
			break;
		case 1:
			NewColor = FLinearColor(0.0f, 1.0f, 0.0f);
			break;
		case 2:
			NewColor = FLinearColor(0.0f, 0.0f, 1.0f);
			break;
		case 3:
			NewColor = FLinearColor(1.0f, 1.0f, 0.0f);
			break;
		}
		MaterialInstance->SetVectorParameterValue(TEXT("BaseColor"), NewColor);
	}
}

void AFuseBox::ChangeCompleteColor()
{
	TArray<UStaticMeshComponent*> mesh = GetMeshComponent();

	UMaterialInterface* Material = mesh[0]->GetMaterial(0);
	UMaterialInstanceDynamic* MaterialInstance = nullptr;
	if (Material)
		MaterialInstance = Cast<UMaterialInstanceDynamic>(Material);
	float max = 1.0f;
	if (MaterialInstance)
		MaterialInstance->SetScalarParameterValue(TEXT("Emissive"), max);
}

void AFuseBox::ChangeActivateEmissiveColor(float value)
{
	TArray<UStaticMeshComponent*> mesh = GetMeshComponent();

	UMaterialInterface* Material = mesh[4]->GetMaterial(0);
	UMaterialInstanceDynamic* MaterialInstance = nullptr;
	if (Material)
		MaterialInstance = Cast<UMaterialInstanceDynamic>(Material);
	if (MaterialInstance)
		MaterialInstance->SetScalarParameterValue(TEXT("Emissive"), value);
}

void AFuseBox::ActivateFuseBox()
{
	complete = true;
}
bool AFuseBox::CheckFuseBoxActivate()
{
	return complete;
}
float AFuseBox::GetFuseBoxCurrentProgress()
{
	return CurrentProgressBarValue;
}

void AFuseBox::SetOpenedStatus(bool open)
{
	fuseBoxOpen = open;
}
void AFuseBox::GetOpenedStatus(bool& open)
{
	open = fuseBoxOpen;
}
void AFuseBox::OpenFuseBox()
{
	ProcessCustomEvent(FName("PlayFuseBoxOpen"));
	SetOpenedStatus(true);
}
void AFuseBox::ResetFuseBox()
{
	SetOpenedStatus(false);
	StopFillingProgressBar();
	CurrentProgressBarValue = 0;
	ProcessCustomEvent(FName("UpdateOpeningFuseBoxStatusWidget"));

}
void AFuseBox::SetFuseBoxOpenStartPoint(float startpoint)
{
	startPoint = startpoint;
}
void AFuseBox::StartFillingProgressBar()
{
	CurrentProgressBarValue = startPoint;
	GetWorld()->GetTimerManager().SetTimer(ProgressBarTimerHandle, this, &AFuseBox::FillProgressBar, 0.3f, true);
}

void AFuseBox::StopFillingProgressBar()
{
	GetWorld()->GetTimerManager().ClearTimer(ProgressBarTimerHandle);
}

void AFuseBox::ProcessCustomEvent(FName FunctionName)
{
	UFunction* CustomEvent = FindFunction(FunctionName);
	if (CustomEvent) {
		ProcessEvent(CustomEvent, nullptr);
	}
}

void AFuseBox::FillProgressBar()
{
	float MaxProgressBarValue = 1;
	CurrentProgressBarValue += 0.01f;
	ProcessCustomEvent(FName("UpdateOpeningFuseBoxStatusWidget"));
	if (CurrentProgressBarValue >= MaxProgressBarValue) {
		StopFillingProgressBar();
		ProcessCustomEvent(FName("FillMaxOpeningFuseBoxStatusWidget"));
	}
}
