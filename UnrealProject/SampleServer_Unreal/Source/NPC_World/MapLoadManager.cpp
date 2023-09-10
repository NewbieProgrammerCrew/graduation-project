// Fill out your copyright notice in the Description page of Project Settings.


#include "MapLoadManager.h"
#include <fstream>
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFileManager.h"
#include "DesktopPlatform/Public/DesktopPlatformModule.h"


AMapLoadManager::AMapLoadManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}
void AMapLoadManager::BeginPlay()
{
	Super::BeginPlay();
	
}
void AMapLoadManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


class FMapObjectData {
public:
    FString Name;
    FVector Position;
    FVector Rotation;
    FVector Scale;

    FMapObjectData(const FString& InName, const FVector& InPosition, const FVector& InRotation, const FVector& InScale)
        : Name(InName), Position(InPosition), Rotation(InRotation), Scale(InScale) {}
};

TArray<FMapObjectData> LoadedObjects;
void AMapLoadManager::ParseMapFile(const FString& FilePath)
{
    std::ifstream inFile(*FilePath, std::ios::binary);

    if (!inFile.is_open()) {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to open file"));
        return;
    }

    while (inFile) {
        int len = 0;
        inFile.read(reinterpret_cast<char*>(&len), sizeof(int));
        if (len > 0 && len < 100) {

            if (inFile.gcount() != sizeof(int)) {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ERROR Load len"));
                break;
            }

            char* ObjectName = new char[len];
            inFile.read(ObjectName, len);

            if (static_cast<size_t>(inFile.gcount()) != len) {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ERROR Load Name"));
                delete[] ObjectName;
                break;
            }
            //// 위치 읽기
            FVector Position;
            float posx, posy, posz;

            inFile.read(reinterpret_cast<char*>(&posx), sizeof(float));
            inFile.read(reinterpret_cast<char*>(&posz), sizeof(float));
            inFile.read(reinterpret_cast<char*>(&posy), sizeof(float));

            Position = FVector(posx, posy, posz);

            // 회전 읽기
            FVector Rotation;
            float rotX, rotY, rotZ;

            inFile.read(reinterpret_cast<char*>(&rotX), sizeof(float));
            inFile.read(reinterpret_cast<char*>(&rotY), sizeof(float));
            inFile.read(reinterpret_cast<char*>(&rotZ), sizeof(float));
            Rotation = FVector(rotX, rotY, rotZ);

            // 크기 읽기
            FVector Scale;
            float scaleX, scaleY, scaleZ;

            inFile.read(reinterpret_cast<char*>(&scaleX), sizeof(float));
            inFile.read(reinterpret_cast<char*>(&scaleZ), sizeof(float));
            inFile.read(reinterpret_cast<char*>(&scaleY), sizeof(float));
            Scale = FVector(scaleX, scaleY, scaleZ);

            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("ObjectName: %s"), *objNameStr));
            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Position: %f, %f, %f"), Position.X, Position.Y, Position.Z));
            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Rotation: %f, %f, %f"), Rotation.X, Rotation.Y, Rotation.Z));
            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Scale: %f, %f, %f"), Scale.X, Scale.Y, Scale.Z));

            FString objNameStr(ObjectName);
            LoadedObjects.Emplace(objNameStr, Position, Rotation, Scale);
            delete[] ObjectName;
        }
        else {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ERROR Load len"));
            break;
        }
    }
    UWorld* World = GetWorld();
    if (World)  SpawnObjectsInWorld(World);
}

void AMapLoadManager::SpawnObjectsInWorld(UWorld* World) {
    for (const FMapObjectData& ObjectData : LoadedObjects) {
        FString BPPath = FString::Printf(TEXT("/Game/Blueprints/MyActor/My%s.My%s_C"), *ObjectData.Name, *ObjectData.Name);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, BPPath);
        UClass* MyActorClass = LoadClass<AActor>(nullptr, *BPPath);

        if (!MyActorClass) {

            continue;
        }

        FRotator RotationAsRotator(ObjectData.Rotation.X, ObjectData.Rotation.Y, ObjectData.Rotation.Z);
        FQuat QuatRotation = FQuat(RotationAsRotator);
        FTransform SpawnTransform(QuatRotation, 100 * ObjectData.Position, ObjectData.Scale);

        AActor* SpawnedActor = World->SpawnActor<AActor>(MyActorClass, SpawnTransform);

        if (SpawnedActor) {
           
        }
    }
}

void AMapLoadManager::OpenFilePicker()
{
    TArray<FString> OutFiles;
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (DesktopPlatform)
    {
        uint32 SelectionFlag = 0;
        DesktopPlatform->OpenFileDialog(
            NULL,
            TEXT("Open"),
            TEXT(""),
            TEXT(""),
            TEXT("User Map files (*.uNPC)|*.uNPC"),
            SelectionFlag,
            OutFiles
        );

    }
    if (OutFiles.Num() > 0) {
        SelectFilePath = OutFiles[0];
        AMapLoadManager::ParseMapFile(SelectFilePath);
    }
}