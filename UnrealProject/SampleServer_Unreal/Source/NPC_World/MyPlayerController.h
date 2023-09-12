#pragma once

#include "CoreMinimal.h"
#include "../../../../Server/ServerTest/ServerTest/protocol.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

UCLASS()
class NPC_WORLD_API AMyPlayerController: public APlayerController
{
    GENERATED_BODY()

public:
    AMyPlayerController();
    virtual void Tick(float DeltaTime) override;
    virtual void SetupInputComponent() override;
    float CalculateSpeed(float deltaTime);

    short direction = -1;
    bool Key_w;
    bool Key_a;
    bool Key_s;
    bool Key_d;

    float TurnValue = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Target_speed = 0;
    bool zero_speed = false;

    void InputFwdPressed();
    void InputBackPressed();
    void InputLeftPressed();
    void InputRightPressed();

    void InputFwdReleased();
    void InputBackReleased();
    void InputLeftReleased();
    void InputRightReleased();

    void Turn(float value);
    void LookUp(float value);

    void SendMovePacket();
    int id;

    class FSocketThread* Network;
    class AMain* _Main;
    FVector LastFrameLocation;
};