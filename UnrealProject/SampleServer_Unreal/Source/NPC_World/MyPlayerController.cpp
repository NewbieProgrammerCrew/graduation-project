#include "MyPlayerController.h"
#include "Kismet/GameplayStatics.h"

#include <string>
#include "DataUpdater.h"
#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
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
    
    if (Key_w || Key_a || Key_s || Key_d) {
        MoveRight(m_Ydir);
        MoveForward(m_Xdir);
        zero_speed = false;

        SendMovePacket();
        UpdateSpeed();
    }
    else{
        CurrentSpeed = 0;
        
        APawn* ControlledPawn = GetPawn();
        if (ControlledPawn) {
            UDataUpdater* DataUpdater = Cast<UDataUpdater>(ControlledPawn->GetComponentByClass(UDataUpdater::StaticClass()));
            if (DataUpdater && !zero_speed) {
              DataUpdater->UpdateSpeedData(CurrentSpeed);
                SendMovePacket();
                zero_speed = true;
            }
        }
    }
}
void AMyPlayerController::UpdateSpeed() 
{
    APawn* ControlledPawn = GetPawn();
    if (ControlledPawn) {
        UFloatingPawnMovement* MovementComponent =
            Cast<UFloatingPawnMovement>(ControlledPawn->GetMovementComponent());
        if (MovementComponent) {
            CurrentSpeed = MovementComponent->Velocity.Size();
        }
        UDataUpdater* DataUpdater = Cast<UDataUpdater>(
            ControlledPawn->GetComponentByClass(UDataUpdater::StaticClass()));
        if (DataUpdater) {
            DataUpdater->UpdateSpeedData(CurrentSpeed);
        }
    }
}
void AMyPlayerController::SendMovePacket()
{
    if (Network) {
        //direction = -1;
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
      
        FVector CurrentPos = ControlledPawn->GetActorLocation();
        CS_MOVE_PACKET packet;
        packet.size = sizeof(CS_MOVE_PACKET);
        packet.x = CurrentPos.X;
        packet.y = CurrentPos.Y;
        packet.z = CurrentPos.Z;

        packet.rx = rx;
        packet.ry = ry;
        packet.rz = rz;
        packet.speed = CurrentSpeed;
        packet.type = CS_MOVE;
        WSA_OVER_EX* wsa_over_ex = new WSA_OVER_EX(OP_SEND, packet.size, &packet);
        WSASend(Network->s_socket, &wsa_over_ex->_wsabuf, 1, 0, 0, &wsa_over_ex->_wsaover, send_callback);
    
    }
}
void AMyPlayerController::InputFwdPressed()
{
    m_Xdir = 1.f;
    Key_w = true;
}
void AMyPlayerController::InputBackPressed()
{
    m_Xdir = -1.f;
    Key_s = true;
}
void AMyPlayerController::InputLeftPressed()
{
    m_Ydir = -1.f;
    Key_a = true;
}
void AMyPlayerController::InputRightPressed()
{
    m_Ydir = 1.f;
    Key_d = true;
}



void AMyPlayerController::InputFwdReleased()
{
    m_Xdir = 0.f;
    Key_w = false;
}

void AMyPlayerController::InputBackReleased()
{
    m_Xdir = 0.f;
    Key_s = false;
}

void AMyPlayerController::InputLeftReleased()
{
    m_Ydir = 0.f;
    Key_a = false;
}

void AMyPlayerController::InputRightReleased()
{
    m_Ydir = 0.f;
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
void AMyPlayerController::MoveForward(float Value) {
    if (Value != 0.0f) {
        
        APawn* ControlledPawn = GetPawn();
        if (ControlledPawn) {
            FVector Direction = ControlledPawn->GetActorForwardVector();
            ControlledPawn->AddMovementInput(Direction, Value);
        }
    }
    GEngine->AddOnScreenDebugMessage(
        -1, 5.f, FColor::Red,
        FString::Printf(TEXT("MoveForward called with Value: %f"), Value));
}

void AMyPlayerController::MoveRight(float Value) {
    if (Value != 0.0f) {
        APawn* ControlledPawn = GetPawn();
        if (ControlledPawn) {
            FVector Direction = ControlledPawn->GetActorRightVector();
            ControlledPawn->AddMovementInput(Direction, Value);
        }
        GEngine->AddOnScreenDebugMessage(
            -1, 5.f, FColor::Red,
            FString::Printf(TEXT("MoveForward called with Value: %f"), Value));

    }
}
void AMyPlayerController::Turn(float Value)
{
    if (Value != 0.0f) {
        TurnValue = Value;
        SendMovePacket();
    }
    else TurnValue = 0.f;
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
