// Fill out your copyright notice in the Description page of Project Settings.


#include "ExportCollisions/ExportAllFuseBoxInfo.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

// Sets default values
AExportAllFuseBoxInfo::AExportAllFuseBoxInfo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AExportAllFuseBoxInfo::BeginPlay()
{
    Super::BeginPlay();
   
}

FString AExportAllFuseBoxInfo::GetExportPath() const
{
    UWorld* World = GetWorld();
    if (World) {
        FString WorldName = World->GetName();
        return FPaths::ProjectDir() + TEXT("Stage") + WorldName + TEXT("FuseBoxCollision.json");
    }
    else {
        return FPaths::ProjectDir() + TEXT("ExportedCollision.json");
    }
}

void AExportAllFuseBoxInfo::WriteFuseBoxInfoToJson()
{
    FString LoadedContent;
    FString Path = GetExportPath();

    TSharedPtr<FJsonObject> RootObject;

    RootObject = MakeShareable(new FJsonObject);
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), FuseBoxActor->GetClass(), FoundActors);
    FoundActors.Sort([&](const AActor& A, const AActor& B) {
        const AFuseBox* ABox = Cast<AFuseBox>(&A);
        const AFuseBox* BBox = Cast<AFuseBox>(&B);
        return ABox && BBox && ABox->GetIndex() < BBox->GetIndex();
        });

    for (AActor* FoundActor : FoundActors) {
        TArray<UBoxComponent*> Components;
        TArray<UCapsuleComponent*> CapsuleComponents;
        FoundActor->GetComponents<UBoxComponent>(Components, true);
        if (Components.Num()) {

            // 액터에 대한 고유 식별자 생성
            FString UniqueID = FGuid::NewGuid().ToString();
            FString ActorDataPath = FString::Printf(TEXT("%s_%s"), *FoundActor->GetName(), *UniqueID);

            TArray<TSharedPtr<FJsonValue>> ActorCollisionDataArray;

            for (UBoxComponent* Component : Components) {
                TSharedPtr<FJsonObject> CollisionObject = MakeShareable(new FJsonObject);
                FTransform Transform = Component->GetComponentTransform();
                FVector Location = Transform.GetLocation();
                FRotator Rotation = Transform.Rotator();

                FVector Extent = Component->GetScaledBoxExtent();

                CollisionObject->SetNumberField("Type", 1);
                CollisionObject->SetNumberField("LocationX", Location.X);
                CollisionObject->SetNumberField("LocationY", Location.Y);
                CollisionObject->SetNumberField("LocationZ", Location.Z);
                CollisionObject->SetNumberField("ExtentX", Extent.X);
                CollisionObject->SetNumberField("ExtentY", Extent.Y);
                CollisionObject->SetNumberField("ExtentZ", Extent.Z);
                CollisionObject->SetNumberField("Yaw", Rotation.Yaw);
                CollisionObject->SetNumberField("Roll", Rotation.Roll);
                CollisionObject->SetNumberField("Pitch", Rotation.Pitch);
                int idx = Cast<AFuseBox>(FoundActor)->GetIndex();
                CollisionObject->SetNumberField("index", idx);

                ActorCollisionDataArray.Add(MakeShareable(new FJsonValueObject(CollisionObject)));
            }
            RootObject->SetArrayField(ActorDataPath, ActorCollisionDataArray);
            Components.Empty();
        }
    }


    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);

    FFileHelper::SaveStringToFile(OutputString, *Path);
}

void AExportAllFuseBoxInfo::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
