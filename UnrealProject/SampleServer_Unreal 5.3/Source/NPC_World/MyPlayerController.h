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
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupInputComponent() override;

    void UpdateSpeed();
    
    // Pressed
    // Keyboard 
    void InputFwdPressed();
    void InputBackPressed();
    void InputLeftPressed();
    void InputRightPressed();
 
    // Mouse
    void LeftMousePressed();

    // Released
    // Keyboard
    void InputFwdReleased();
    void InputBackReleased();
    void InputLeftReleased();
    void InputRightReleased();
 
    // Mouse
    void LeftMouseReleased();

    // Movement
    void MoveForward(float Value);
    void MoveRight(float Value);
    void StartRunning();
    void StopRunning();
    void Turn(float value);
    void LookUp(float value);

    // setter
    void SetCurrSpeed(float speed); 
    // getter
    float GetCurrSpeed();

    void SendMovePacket();
    class FSocketThread* Network;
    class AMain* _Main;
    int id;

private:
    float m_Xdir = 0.f;
    float m_Ydir = 0.f;
    float TurnValue = 0;

    bool zero_speed = false;

    bool Key_w;
    bool Key_a;
    bool Key_s;
    bool Key_d;

    float m_CurrSpeed = 0;
    float RunningSpeed = 1800.0f;
    float WalkingSpeed = 600.0f;

};