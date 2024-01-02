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

int AFuseBox::GetColorId(int c)
{
	return color_id;
}

void AFuseBox::ChangeBaseColor()
{
	TArray<UStaticMeshComponent*> mesh = GetMeshComponent();
	for (int i{}; i < mesh.Num() - 1; ++i) {
		UMaterialInterface* Material = mesh[i]->GetMaterial(2);
		UMaterialInstanceDynamic* MaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
		mesh[i]->SetMaterial(2, MaterialInstance);
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
	UMaterialInstanceDynamic* MaterialInstance = Cast<UMaterialInstanceDynamic>(mesh[0]->GetMaterial(0));
	float max = 1.0f;
	MaterialInstance->SetScalarParameterValue(TEXT("Emissive"), max);
}

void AFuseBox::UpdateFuseBoxProgressStatus(bool status)
{
	complete = status;
}

bool AFuseBox::GetFuseBoxProgressStatus()
{
	return complete;
}

void AFuseBox::SetColorId(int c)
{
	color_id = c;
}

