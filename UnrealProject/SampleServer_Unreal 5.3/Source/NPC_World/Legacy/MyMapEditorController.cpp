// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMapEditorController.h"
#include "ProceduralTerrainActor.h"
#include "MapEditorMode.h"

struct GridCell {
    TArray<FVector> Vertices;
};

AMyMapEditorController::AMyMapEditorController()
{
    PrimaryActorTick.bCanEverTick = true;
	MapEditorMode = nullptr;
    bLeftMousePressed = false;
}
void AMyMapEditorController::BeginPlay()
{
    AGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AGameModeBase>();
    MapEditorMode = Cast<AMapEditorMode>(GameMode);
}
void AMyMapEditorController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bLeftMousePressed) {
        AdjustHeight(DeltaTime);
    }
}
void AMyMapEditorController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("LeftMouse", IE_Pressed, this, &AMyMapEditorController::HandleLeftClick);
	InputComponent->BindAction("LeftMouse", IE_Released, this, &AMyMapEditorController::HandleLeftClickReleased);
}

void AMyMapEditorController::HandleLeftClick()
{
    if (MapEditorMode->GetCurrentMode() == 1) {   // Landscape mode
        bLeftMousePressed = true;
    }
    else {

    }
}

void AMyMapEditorController::AdjustHeight(float DeltaTime)
{
    FVector2D MousePosition;
    GetMousePosition(MousePosition.X, MousePosition.Y);
    // 3D 월드 좌표로 변환
    FVector WorldLocation, WorldDirection;
    DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldLocation, WorldDirection);
    FVector Start = PlayerCameraManager->GetCameraLocation();
    FVector End = Start + (WorldDirection * 1000.f);
    FCollisionQueryParams CollisionParams;
    FHitResult HitResult;

    if (MapEditorMode->GetCurrentMode() == 1) {        // Landscape mode
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("Left Mouse Click! and Mode is Terrain")));
        bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
        if (bHit) {
            AActor* HitActor = HitResult.GetActor();
            AProceduralTerrainActor* PTerrain = Cast<AProceduralTerrainActor>(HitActor);
            if (PTerrain) {
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("its Terrain!!!")));
                TArray<FVector>& ProceduralVertices = PTerrain->m_Vertices;
                FVector HitLocation = HitResult.Location;
                FVector LocalHitLocation = HitActor->GetTransform().InverseTransformPosition(HitResult.Location);
                float MinDistance = FLT_MAX;
                int32 ClosestVertexIndex = -1;
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("%d"), ProceduralVertices.Num()));
                for (int32 i = 0; i < ProceduralVertices.Num(); ++i) {
                    float Distance = FVector::DistSquared(LocalHitLocation, ProceduralVertices[i]);
                    if (Distance < MinDistance) {
                        MinDistance = Distance;
                        ClosestVertexIndex = i;
                    }
                }

                if (ClosestVertexIndex != -1) {
                    float Radius = 5.0f;  // 원의 반지름
                    float Height = 100.0f * DeltaTime;
                    float RadiusSquared = Radius * Radius;

                    for (int32 i = 0; i < ProceduralVertices.Num(); ++i) {
                        FVector2D Diff = FVector2D(ProceduralVertices[i].X - ProceduralVertices[ClosestVertexIndex].X,
                            ProceduralVertices[i].Y - ProceduralVertices[ClosestVertexIndex].Y);
                        float DistanceSquared = Diff.SizeSquared();
                        if (DistanceSquared <= RadiusSquared) {
                            float Factor = FMath::Exp(-DistanceSquared / (2.0f * RadiusSquared / 4.0f));  // 가우스 함수
                            ProceduralVertices[i].Z += Height * Factor;
                        }
                    }

                    PTerrain->UpdateMeshSection(ProceduralVertices);
                }
            }
        }
    }
}
void AMyMapEditorController::HandleLeftClickReleased()
{
    bLeftMousePressed = false;
}
