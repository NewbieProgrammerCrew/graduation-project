#include "ExportCollision.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
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
                    // 꼭지점의 월드 위치 계산 및 JSON에 추가

                    TArray<TSharedPtr<FJsonValue>> CornersArray;
                    TArray<FVector> Corners = {
                        FVector(+Extent.X, +Extent.Y, +Extent.Z),
                        FVector(+Extent.X, +Extent.Y, -Extent.Z),
                        FVector(+Extent.X, -Extent.Y, +Extent.Z),
                        FVector(+Extent.X, -Extent.Y, -Extent.Z),
                        FVector(-Extent.X, +Extent.Y, +Extent.Z),
                        FVector(-Extent.X, +Extent.Y, -Extent.Z),
                        FVector(-Extent.X, -Extent.Y, +Extent.Z),
                        FVector(-Extent.X, -Extent.Y, -Extent.Z)
                    };

                    for (FVector& Corner : Corners) {
                        TSharedPtr<FJsonObject> CornerObject = MakeShareable(new FJsonObject);
                        CornerObject->SetNumberField("X", Location.X + Corner.X);
                        CornerObject->SetNumberField("Y", Location.Y + Corner.Y);
                        CornerObject->SetNumberField("Z", Location.Z + Corner.Z);
                        CornersArray.Add(MakeShareable(new FJsonValueObject(CornerObject)));
                    }

                    CollisionObject->SetArrayField("Corners", CornersArray);
                    ActorCollisionDataArray.Add(MakeShareable(new FJsonValueObject(CollisionObject)));
                }
                RootObject->SetArrayField(ActorDataPath, ActorCollisionDataArray);
                Components.Empty();
            }
            else {
                FoundActor->GetComponents<UCapsuleComponent>(CapsuleComponents, true);
                if (CapsuleComponents.Num()) {
                    // 액터에 대한 고유 식별자 생성
                    FString UniqueID = FGuid::NewGuid().ToString();
                    FString ActorDataPath = FString::Printf(TEXT("%s_%s"), *FoundActor->GetName(), *UniqueID);

                    TArray<TSharedPtr<FJsonValue>> ActorCollisionDataArray;

                    for (UCapsuleComponent* Component : CapsuleComponents) {
                        TSharedPtr<FJsonObject> CollisionObject = MakeShareable(new FJsonObject);
                        FTransform Transform = Component->GetComponentTransform();
                        FVector Location = Transform.GetLocation();
                        FRotator Rotation = Transform.Rotator();
                        float Radius, HalfHeight;
                        Component->GetScaledCapsuleSize(Radius, HalfHeight);

                        CollisionObject->SetStringField("Type", "Capsule");
                        CollisionObject->SetNumberField("LocationX", Location.X);
                        CollisionObject->SetNumberField("LocationY", Location.Y);
                        CollisionObject->SetNumberField("LocationZ", Location.Z);
                        CollisionObject->SetNumberField("ExtentX", Radius);
                        CollisionObject->SetNumberField("ExtentZ", HalfHeight);
                        CollisionObject->SetNumberField("Yaw", Rotation.Yaw);
                        CollisionObject->SetNumberField("Roll", Rotation.Roll);
                        CollisionObject->SetNumberField("Pitch", Rotation.Pitch);

                        ActorCollisionDataArray.Add(MakeShareable(new FJsonValueObject(CollisionObject)));
                    }
                    RootObject->SetArrayField(ActorDataPath, ActorCollisionDataArray);
                    CapsuleComponents.Empty();
                }

            }
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
