#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExportCollision.generated.h"

UCLASS()
class NPC_WORLD_API AExportCollision : public AActor
{
    GENERATED_BODY()

public:
    AExportCollision();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Export")
    TArray<AActor*> ActorsToExport;

private:
    FString GetExportPath() const;
};
