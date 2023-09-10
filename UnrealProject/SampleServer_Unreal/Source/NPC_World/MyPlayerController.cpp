#include "MyPlayerController.h"
#include "Kismet/GameplayStatics.h"

#include <string>

#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "NetworkingThread.h"



AMyPlayerController::AMyPlayerController()
{

    PrimaryActorTick.bCanEverTick = true;

    UWorld* worldref = GetWorld();
    if (worldref == nullptr) return;

    AActor* actor = UGameplayStatics::GetActorOfClass(worldref, AMain::StaticClass());
    if (actor == nullptr) return;

    _Main = Cast<AMain>(actor);
    if (_Main == nullptr) return;

    Key_w = false;
    Key_a = false;
    Key_s = false;
    Key_d = false;
}


void AMyPlayerController::Tick(float DeltaTime)
{
    if (Network == nullptr) {
        Network = reinterpret_cast<FSocketThread*>(_Main->Network);
        Network->_MyController = this;
    }
    if (Key_w || Key_a || Key_s || Key_d) SendMovePacket();
    Speed = CalculateSpeed(DeltaTime);
}
float AMyPlayerController::CalculateSpeed(float DeltaTime)
{
    APawn* ControlledPawn = GetPawn();
    float NewSpeed = 0;
    if (ControlledPawn) {
        FVector CurrentFrameLocation = ControlledPawn->GetActorLocation();
        NewSpeed = (CurrentFrameLocation - LastFrameLocation).Size() / DeltaTime;
        LastFrameLocation = CurrentFrameLocation;
    }
    return NewSpeed;
}
void AMyPlayerController::SendMovePacket()
{
    if (Network) {
        direction = -1;
        APawn* ControlledPawn = GetPawn();
        float rx = 0;
        float ry = 0;
        float rz = 0;
        if (ControlledPawn) {
            FRotator CurrentRotation = ControlledPawn->GetActorRotation();
            rx = CurrentRotation.Pitch;
            ry = CurrentRotation.Yaw + TurnValue;
            rz = CurrentRotation.Roll;
        }
        if (Key_w)
            direction = 0;
        if (Key_s)
            direction = 1;
        if (Key_a)
            direction = 2;
        if (Key_d)
            direction = 3;

        CS_MOVE_PACKET packet;
        packet.size = sizeof(CS_MOVE_PACKET);
        packet.direction = direction;
        packet.rx = rx;
        packet.ry = ry;
        packet.rz = rz;
        packet.speed = Speed;
        packet.type = CS_MOVE;
        WSA_OVER_EX* wsa_over_ex = new WSA_OVER_EX(OP_SEND, packet.size, &packet);
        WSASend(Network->s_socket, &wsa_over_ex->_wsabuf, 1, 0, 0, &wsa_over_ex->_wsaover, send_callback);

    }
}
void AMyPlayerController::InputFwdPressed()
{
    Key_w = true;
}
void AMyPlayerController::InputBackPressed()
{
    Key_s = true;
}
void AMyPlayerController::InputLeftPressed()
{
    Key_a = true;
}
void AMyPlayerController::InputRightPressed()
{
    Key_d = true;
}



void AMyPlayerController::InputFwdReleased()
{
    Key_w = false;
}

void AMyPlayerController::InputBackReleased()
{
    Key_s = false;
}

void AMyPlayerController::InputLeftReleased()
{
    Key_a = false;
}

void AMyPlayerController::InputRightReleased()
{
    Key_d = false;
}


void AMyPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    InputComponent->BindAction("MoveForward", IE_Pressed, this, &AMyPlayerController::InputFwdPressed);
    InputComponent->BindAction("MoveForward", IE_Released, this, &AMyPlayerController::InputFwdReleased);

    InputComponent->BindAction("MoveBackward", IE_Pressed, this, &AMyPlayerController::InputBackPressed);
    InputComponent->BindAction("MoveBackward", IE_Released, this, &AMyPlayerController::InputBackReleased);

    InputComponent->BindAction("MoveLeft", IE_Pressed, this, &AMyPlayerController::InputLeftPressed);
    InputComponent->BindAction("MoveLeft", IE_Released, this, &AMyPlayerController::InputLeftReleased);

    InputComponent->BindAction("MoveRight", IE_Pressed, this, &AMyPlayerController::InputRightPressed);
    InputComponent->BindAction("MoveRight", IE_Released, this, &AMyPlayerController::InputRightReleased);

    InputComponent->BindAxis("MouseX", this, &AMyPlayerController::Turn);
    InputComponent->BindAxis("MouseY", this, &AMyPlayerController::LookUp);
}
void AMyPlayerController::Turn(float Value)
{
    if (Value != 0.0f) {
        TurnValue = Value;
        SendMovePacket();
    }
}

void AMyPlayerController::LookUp(float Value)
{
    if (Value != 0.0f && GetPawn()) {
        USpringArmComponent* SpringArm = Cast<USpringArmComponent>(GetPawn()->GetComponentByClass(USpringArmComponent::StaticClass()));
        if (SpringArm) {
            FRotator NewRotation = SpringArm->GetComponentRotation();
            NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch - Value, -80.0f, 80.0f);
            SpringArm->SetWorldRotation(NewRotation);
        }
    }
}
