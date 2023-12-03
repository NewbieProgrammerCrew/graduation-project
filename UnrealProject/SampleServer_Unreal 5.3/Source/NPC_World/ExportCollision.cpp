#include "ExportCollision.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

AExportCollision::AExportCollision()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AExportCollision::BeginPlay()
{
    Super::BeginPlay();

    FString LoadedContent;
    FString Path = GetExportPath();

    TSharedPtr<FJsonObject> RootObject;
    
    RootObject = MakeShareable(new FJsonObject);
    for (AActor* Actor : ActorsToExport) {
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), Actor->GetClass(), FoundActors);

        for (AActor* FoundActor : FoundActors) {
            TArray<UBoxComponent*> Components;
            FoundActor->GetComponents<UBoxComponent>(Components, true);

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

                CollisionObject->SetStringField("Type", "Box");
                CollisionObject->SetNumberField("LocationX", Location.X);
                CollisionObject->SetNumberField("LocationY", Location.Y);
                CollisionObject->SetNumberField("LocationZ", Location.Z);
                CollisionObject->SetNumberField("ExtentX", Extent.X);
                CollisionObject->SetNumberField("ExtentY", Extent.Y);
                CollisionObject->SetNumberField("ExtentZ", Extent.Z);
                CollisionObject->SetNumberField("Yaw", Rotation.Yaw);
                CollisionObject->SetNumberField("Roll", Rotation.Roll);
                CollisionObject->SetNumberField("Pitch", Rotation.Pitch);

                ActorCollisionDataArray.Add(MakeShareable(new FJsonValueObject(CollisionObject)));
            }
            RootObject->SetArrayField(ActorDataPath, ActorCollisionDataArray);
        }
    }

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);

    FFileHelper::SaveStringToFile(OutputString, *Path);
}

FString AExportCollision::GetExportPath() const
{
    UWorld* World = GetWorld();
    if (World) {
        FString WorldName = World->GetName();
        return FPaths::ProjectDir() + TEXT("Stage") + WorldName + TEXT("Collision.json");
    }
    else {
        return FPaths::ProjectDir() + TEXT("ExportedCollision.json");
    }
}

void AExportCollision::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
