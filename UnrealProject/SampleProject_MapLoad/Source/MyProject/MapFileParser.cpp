// Fill out your copyright notice in the Description page of Project Settings.


#include "MapFileParser.h"
#include <fstream>
#include "HAL/PlatformFilemanager.h"

UMapFileParser::UMapFileParser(){}

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
void UMapFileParser::ParseMapFile(const FString& FilePath)
{
    std::ifstream inFile(*FilePath, std::ios::binary);

    if (!inFile.is_open())
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to open file"));
        return;
    }

    while (inFile)
    {
        int len = 0;
        inFile.read(reinterpret_cast<char*>(&len), sizeof(int));
        if (len > 0 && len < 100) {

            if (inFile.gcount() != sizeof(int))
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ERROR Load len"));
                break;
            }

            char* ObjectName = new char[len];
            inFile.read(ObjectName, len);

            if (static_cast<size_t>(inFile.gcount()) != len)
            {
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


            ////// 여기에서 읽은 데이터를 원하는 방식으로 사용합니다.
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
    if (World)
        SpawnObjectsInWorld(World);
}

void UMapFileParser::SpawnObjectsInWorld(UWorld* World) {
    for (const FMapObjectData& ObjectData : LoadedObjects) {


        FString BPPath = FString::Printf(TEXT("/Game/Dynamic/MyActor/My%s.My%s_C"), *ObjectData.Name, *ObjectData.Name);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, BPPath);

        UClass* MyActorClass = LoadClass<AActor>(nullptr, *BPPath);

        if (!MyActorClass) {
        
            continue;
        }

        FRotator RotationAsRotator(ObjectData.Rotation.X, ObjectData.Rotation.Y, ObjectData.Rotation.Z);
        FQuat QuatRotation = FQuat(RotationAsRotator);
        FTransform SpawnTransform(QuatRotation, 100* ObjectData.Position, ObjectData.Scale);

        AActor* SpawnedActor = World->SpawnActor<AActor>(MyActorClass, SpawnTransform);

        if (SpawnedActor) {
            // 필요한 경우, 스폰된 액터에 대한 추가 설정을 여기에 작성
        }
    }
}
