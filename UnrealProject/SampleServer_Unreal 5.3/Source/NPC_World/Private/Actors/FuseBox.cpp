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
	PlayFuseBoxOpenEvent = FindFunction("PlayFuseBoxOpen");
	PlayFuseBoxCloseEvent = FindFunction("PlayFuseBoxClose");
	UpdateOpeningFuseBoxStatusWidgetEvent = FindFunction("UpdateOpeningFuseBoxStatusWidget");
	FillMaxOpeningFuseBoxStatusWidgetEvent = FindFunction("FillMaxOpeningFuseBoxStatusWidget");

	ChangeInvalidColorEvent = FindFunction("ChangeInvalidColor");
	ChangeValidColorEvent = FindFunction("ChangeValidColor");
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
int AFuseBox::GetColorId()
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
		auto NewColor = color[color_id];
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

void AFuseBox::ChangeValidColor()
{
	ProcessEvent(ChangeValidColorEvent, nullptr);
}

void AFuseBox::ChangeInvalidColor()
{
	ProcessEvent(ChangeInvalidColorEvent, nullptr);
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
	ProcessEvent(PlayFuseBoxOpenEvent, nullptr);
	SetOpenedStatus(true);
}
void AFuseBox::ResetFuseBox()
{
	SetOpenedStatus(false);
	StopFillingProgressBar();
	ProcessEvent(PlayFuseBoxCloseEvent, nullptr);
	CurrentProgressBarValue = 0;
	ProcessEvent(UpdateOpeningFuseBoxStatusWidgetEvent, nullptr);
}
void AFuseBox::SetFuseBoxOpenStartPoint(float startpoint)
{
	startPoint = startpoint;
}
void AFuseBox::StartFillingProgressBar()
{
	CurrentProgressBarValue = startPoint;
	//퓨즈박스 progress 시간 5초로 설정(임시)
	GetWorld()->GetTimerManager().SetTimer(ProgressBarTimerHandle, this, &AFuseBox::FillProgressBar, 0.05f, true);
}

void AFuseBox::StopFillingProgressBar()
{
	GetWorld()->GetTimerManager().ClearTimer(ProgressBarTimerHandle);
}

void AFuseBox::FillProgressBar()
{
	float MaxProgressBarValue = 1;
	CurrentProgressBarValue += 0.01f;
	ProcessEvent(UpdateOpeningFuseBoxStatusWidgetEvent, nullptr);
	if (CurrentProgressBarValue >= MaxProgressBarValue) {
		StopFillingProgressBar();
		ProcessEvent(FillMaxOpeningFuseBoxStatusWidgetEvent, nullptr);
	}
}
