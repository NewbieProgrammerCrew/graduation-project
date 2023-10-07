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
    void UpdateSpeed();

    float m_Xdir = 0.f;
    float m_Ydir = 0.f;
    bool Key_w;
    bool Key_a;
    bool Key_s;
    bool Key_d;

    float TurnValue = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CurrentSpeed = 0;
    bool zero_speed = false;

    void InputFwdPressed();
    void InputBackPressed();
    void InputLeftPressed();
    void InputRightPressed();
    
    void LeftMousePressed();

    void InputFwdReleased();
    void InputBackReleased();
    void InputLeftReleased();
    void InputRightReleased();

    void LeftMouseReleased();

    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float value);
    void LookUp(float value);

    void SendMovePacket();
    int id;

    class FSocketThread* Network;
    class AMain* _Main;
    FVector LastFrameLocation;
};