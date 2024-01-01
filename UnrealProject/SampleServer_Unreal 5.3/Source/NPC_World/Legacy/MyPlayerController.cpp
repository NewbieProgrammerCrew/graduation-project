/*
#include "MyPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include <string>
#include "Main.h"
#include "DataUpdater.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NetworkingThread.h"

AMyPlayerController::AMyPlayerController()
{
    PrimaryActorTick.bCanEverTick = true;
    ControlledPawn = nullptr;
}
void AMyPlayerController::BeginPlay() 
{
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
    key_space = false;
}

void AMyPlayerController::Tick(float DeltaTime)
{
    if (Network == nullptr) {
        //Network = reinterpret_cast<FSocketThread*>(_Main->Network);
        //Network->_MyController = this;
    }
    if (!ControlledPawn) {
        ControlledPawn = GetPawn();
    }
    if (ControlledPawn) {
        if (Key_w || Key_a || Key_s || Key_d || key_space) {
            MoveRight(m_Ydir);
            MoveForward(m_Xdir);
            zero_speed = false;
            //SendMovePacket();
        }
        else {
            m_CurrSpeed = 0;
            if (ControlledPawn) {
                UDataUpdater* DataUpdater = Cast<UDataUpdater>(ControlledPawn->GetComponentByClass(UDataUpdater::StaticClass()));
                if (DataUpdater && !zero_speed) {
                    DataUpdater->SetCurrentSpeed(m_CurrSpeed);

                    if (DataUpdater->GetRole() == "Runner") {
                        UFunction* AddWidgetEvent = ControlledPawn->FindFunction(FName("AddWidgetEvent"));
                        if (AddWidgetEvent) {
                            ControlledPawn->ProcessEvent(AddWidgetEvent, nullptr);
                        }
                    }
                    zero_speed = true;
                }
            }
        }
    }
    SendMovePacket();
    UpdateSpeed();
}
void AMyPlayerController::UpdateSpeed()
{
    if (!ControlledPawn) {
        ControlledPawn = GetPawn();
    }
    if (ControlledPawn) {
        UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(ControlledPawn->GetMovementComponent());
        if (MovementComponent) {
            m_CurrSpeed = MovementComponent->Velocity.Size();
        }
        UDataUpdater* DataUpdater = Cast<UDataUpdater>(ControlledPawn->GetComponentByClass(UDataUpdater::StaticClass()));
        if (DataUpdater) {
            DataUpdater->SetCurrentSpeed(m_CurrSpeed);
            DataUpdater->GetJumpStatus(key_space);
            DataUpdater->SetOnJumpStatus(IsCharacterFalling());
        }
    }
}
void AMyPlayerController::SetCurrSpeed(float speed)
{
    m_CurrSpeed = speed;
}
float AMyPlayerController::GetCurrSpeed()
{
    return m_CurrSpeed;
}
void AMyPlayerController::SendMovePacket()
{
    if (Network) {
        if (!ControlledPawn) {
            ControlledPawn = GetPawn();
        }
        if (ControlledPawn) {
            float rx = 0;
            float ry = 0;
            float rz = 0;

            FRotator CurrentRotation = ControlledPawn->GetActorRotation();
            rx = CurrentRotation.Pitch;
            ry = CurrentRotation.Yaw + TurnValue;
            rz = CurrentRotation.Roll;

            FVector CurrentPos = ControlledPawn->GetActorLocation();
            CS_MOVE_PACKET packet;
            packet.size = sizeof(CS_MOVE_PACKET);
            packet.x = CurrentPos.X;
            packet.y = CurrentPos.Y;
            packet.z = CurrentPos.Z;

            packet.rx = rx;
            packet.ry = ry;
            packet.rz = rz;
            packet.speed = m_CurrSpeed;
            packet.jump = key_space;
            packet.type = CS_MOVE;

            WSA_OVER_EX* wsa_over_ex = new (std::nothrow) WSA_OVER_EX(OP_SEND, packet.size, &packet);
            if (!wsa_over_ex) {
                return;
            }

            if (WSASend(Network->s_socket, &wsa_over_ex->_wsabuf, 1, 0, 0, &wsa_over_ex->_wsaover, send_callback) == SOCKET_ERROR) {
                int error = WSAGetLastError();
                delete wsa_over_ex;
            }
        }
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

void AMyPlayerController::InputSpacePressed()
{
    if (!key_space) {
        if (!ControlledPawn) {
            ControlledPawn = GetPawn();
        }
        ACharacter* MyCharacter = Cast<ACharacter>(ControlledPawn);

        if (MyCharacter) {
            MyCharacter->Jump();
            key_space = true;
            /* UFunction* JumpAnimEvent = MyCharacter->FindFunction(FName("JumpAnimEvent"));
             if (JumpAnimEvent) {
                 MyCharacter->ProcessEvent(JumpAnimEvent, nullptr);
                 }
           
        }
    }

}

void AMyPlayerController::InputESCPressed()
{
    UWorld* World = GetWorld();
    if (World){
        APlayerController* PlayerController = World->GetFirstPlayerController();
        UKismetSystemLibrary::QuitGame(World, PlayerController, EQuitPreference::Quit, true);
    }
}

void AMyPlayerController::SendHitPacket()
{
}
void AMyPlayerController::LeftMousePressed() // ¿ÞÂÊ ¹öÆ° 
{
    if (!ControlledPawn) {
        ControlledPawn = GetPawn();
    }
    UDataUpdater* DataUpdater = nullptr;
    if (ControlledPawn) {
        DataUpdater = Cast<UDataUpdater>(ControlledPawn->GetComponentByClass(UDataUpdater::StaticClass()));
        if (DataUpdater && DataUpdater->GetRole() == "Chaser") {
            CS_ATTACK_PACKET packet;
            FVector pos = ControlledPawn->GetActorLocation();

            FRotator CurrentRotation = ControlledPawn->GetActorRotation();
            float rx = CurrentRotation.Pitch;
            float ry = CurrentRotation.Yaw + TurnValue;
            float rz = CurrentRotation.Roll;

            packet.size = sizeof(CS_ATTACK_PACKET);
            packet.id = id;

            packet.x = pos.X;
            packet.y = pos.Y;
            packet.z = pos.Z;

            packet.rx = rx;
            packet.ry = ry;
            packet.rz = rz;

            packet.type = CS_ATTACK;

            WSA_OVER_EX* wsa_over_ex = new (std::nothrow) WSA_OVER_EX(OP_SEND, packet.size, &packet);
            if (!wsa_over_ex) {
                return;
            }

            if (WSASend(Network->s_socket, &wsa_over_ex->_wsabuf, 1, 0, 0,
                &wsa_over_ex->_wsaover, send_callback) == SOCKET_ERROR) {
                int error = WSAGetLastError();
                delete wsa_over_ex;
            }
        }
    }
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

void AMyPlayerController::InputSpaceReleased()
{
    /*
    APawn* ControlledPawn = GetPawn();
    if (ControlledPawn) {
        ACharacter* MyCharacter = Cast<ACharacter>(ControlledPawn);
        if (MyCharacter) {
           MyCharacter->StopJumping();
           key_space = false;
        }
    }
}
void AMyPlayerController::LeftMouseReleased() 
{ 

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

    InputComponent->BindAction("Jump", IE_Pressed, this, &AMyPlayerController::InputSpacePressed);
    InputComponent->BindAction("Jump", IE_Released, this, &AMyPlayerController::InputSpaceReleased);

    InputComponent->BindAction("Run", IE_Pressed, this, &AMyPlayerController::StartRunning);
    InputComponent->BindAction("Run", IE_Released, this, &AMyPlayerController::StopRunning);

    InputComponent->BindAction("Escape", IE_Pressed, this, &AMyPlayerController::InputESCPressed);

    InputComponent->BindAction("LeftMouse", IE_Pressed, this, &AMyPlayerController::LeftMousePressed);
    InputComponent->BindAction("LeftMouse", IE_Released, this, &AMyPlayerController::LeftMouseReleased);

    InputComponent->BindAxis("MouseX", this, &AMyPlayerController::Turn);
    InputComponent->BindAxis("MouseY", this, &AMyPlayerController::LookUp);
}
void AMyPlayerController::MoveForward(float Value) {
    if (Value != 0.0f) {
        if (!ControlledPawn) {
            ControlledPawn = GetPawn();
        }
        FVector Direction = ControlledPawn->GetActorForwardVector();
        ControlledPawn->AddMovementInput(Direction, Value);

    }
}

void AMyPlayerController::MoveRight(float Value) {
    if (Value != 0.0f) {
        if (!ControlledPawn) {
            ControlledPawn = GetPawn();
        }
        FVector Direction = ControlledPawn->GetActorRightVector();
        ControlledPawn->AddMovementInput(Direction, Value);

    }
}
void AMyPlayerController::StartRunning()
{
    if (!ControlledPawn) {
        ControlledPawn = GetPawn();
    }
    ACharacter* MyCharacter = Cast<ACharacter>(ControlledPawn);
    if (MyCharacter) {
        WalkingSpeed = MyCharacter->GetCharacterMovement()->MaxWalkSpeed;
        MyCharacter->GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;

    }
}

void AMyPlayerController::StopRunning()
{
    if (!ControlledPawn) {
        ControlledPawn = GetPawn();
    }
    ACharacter* MyCharacter = Cast<ACharacter>(ControlledPawn);
    if (MyCharacter)
        MyCharacter->GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;

}
void AMyPlayerController::Turn(float Value)
{
    if (FMath::Abs(Value) > 0.001f) {
        TurnValue = Value;
        SendMovePacket();
    }
    else TurnValue = 0.f;
}
void AMyPlayerController::LookUp(float Value)
{
    if (!ControlledPawn) {
        ControlledPawn = GetPawn();
    }
    if (Value != 0.0f &&  ControlledPawn) {
        USpringArmComponent* SpringArm = Cast<USpringArmComponent>(ControlledPawn->GetComponentByClass(USpringArmComponent::StaticClass()));
        if (SpringArm) {
            FRotator NewRotation = SpringArm->GetComponentRotation();
            NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + Value, -80.0f, 80.0f);
            SpringArm->SetWorldRotation(NewRotation);
        }
    }
}
bool AMyPlayerController::IsCharacterFalling()
{
    if (ControlledPawn) {
        ACharacter* MyCharacter = Cast<ACharacter>(ControlledPawn);
        if (MyCharacter) {
            UCharacterMovementComponent* MovementComponent = MyCharacter->GetCharacterMovement();
            if (MovementComponent) {
                FVector Velocity = MovementComponent->Velocity;
                bool IsMovingUpwards = Velocity.Z > 0.0f; 
                bool IsMovingDownwards = Velocity.Z < 0.0f;
                bool IsOnGround = MovementComponent->IsMovingOnGround();
                if (!IsOnGround && (IsMovingUpwards || IsMovingDownwards)){
                    return true;
                }
            }
        }
    }
    return false;
}
*/