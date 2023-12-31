
#include "Ch_PlayerController.h"

#include "Kismet/GameplayStatics.h"
#include <GameFramework/CharacterMovementComponent.h>

//character
#include "GameFramework/Character.h"
// EnhancedInput
#include "EnhancedInput/Public/InputMappingContext.h"
#include "EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"
#include "EnhancedInput/Public/InputActionValue.h"
//Network
#include "../Main.h"
#include "../NetworkingThread.h"
#include "../PacketExchangeComponent.h"

void ACh_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	UWorld* worldref = GetWorld();
	if (worldref == nullptr) return;

	AActor* actor = UGameplayStatics::GetActorOfClass(worldref, AMain::StaticClass());
	if (actor == nullptr) return;

	m_Main = Cast<AMain>(actor);
	if (m_Main == nullptr) return;
}

void ACh_PlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ACh_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Network == nullptr) {
		Network = reinterpret_cast<FSocketThread*>(m_Main->Network);
		Network->_MyController = this;
	}
}

void ACh_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	// Get the local Player SubSystem
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	// Clear out existing mapping. add our mapping
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);

	UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(InputComponent);

	// Bind the actions

	// Completed: 눌렀다 땠을 때, Triggered 누르고 있을 때

	PEI->BindAction(InputActions->InputMove, ETriggerEvent::Triggered, this, &ACh_PlayerController::Move);
	PEI->BindAction(InputActions->InputMove, ETriggerEvent::Completed, this, &ACh_PlayerController::MoveEnd);

	PEI->BindAction(InputActions->InputSprint, ETriggerEvent::Started, this, &ACh_PlayerController::Sprint);
	PEI->BindAction(InputActions->InputSprint, ETriggerEvent::Completed, this, &ACh_PlayerController::StopSprint);
	PEI->BindAction(InputActions->InputJump, ETriggerEvent::Triggered, this, &ACh_PlayerController::Jump);
	PEI->BindAction(InputActions->InputAttack, ETriggerEvent::Triggered, this, &ACh_PlayerController::Attack);
	PEI->BindAction(InputActions->InputLook, ETriggerEvent::Triggered, this, &ACh_PlayerController::Look);
	PEI->BindAction(InputActions->InputESC, ETriggerEvent::Triggered, this, &ACh_PlayerController::EscapeGame);
}

void ACh_PlayerController::SetId(int id)
{
	m_id = id;
}

int ACh_PlayerController::GetId()
{
	return m_id;
}

void ACh_PlayerController::Move(const FInputActionValue& value)
{
	//input is a Vector 2D
	FVector2D MovementVector = value.Get<FVector2D>();

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FRotationMatrix RotationMatrix = FRotationMatrix(YawRotation);
	
	FVector ForwardDirection;
	FVector RightDirection;
	FVector UpDirection;
	RotationMatrix.GetUnitAxes(ForwardDirection,RightDirection,UpDirection); 

	if (!ControlledPawn) {
		ControlledPawn = GetPawn();
	}
	else {
		ControlledPawn->AddMovementInput(ForwardDirection, MovementVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, MovementVector.X);
	}
	SendMovePacket();
}
void ACh_PlayerController::MoveEnd(const FInputActionValue& value)
{
	SendMovePacket(0);
}
void ACh_PlayerController::SendMovePacket(int speed)
{
	//패킷 전송, 현재 방향,속도, 위치 보낼 것
	//공격 패킷 전송.
	if (!ControlledPawn) {
		ControlledPawn = GetPawn();
	}
	UPacketExchangeComponent* PacketExchange = nullptr;
	if (ControlledPawn) {
		PacketExchange = Cast<UPacketExchangeComponent>(ControlledPawn->GetComponentByClass(UPacketExchangeComponent::StaticClass()));
		PacketExchange->SendMovePacket(speed);
	}
}

void ACh_PlayerController::Sprint(const FInputActionValue& value)
{
	if (!ControlledPawn) {
		ControlledPawn = GetPawn();
	}
	else {
		ACharacter* MyCharacter = Cast<ACharacter>(ControlledPawn);
		if (MyCharacter) {
			WalkingSpeed = MyCharacter->GetCharacterMovement()->MaxWalkSpeed;
			MyCharacter->GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;

		}
	}
}


void ACh_PlayerController::Look(const FInputActionValue& value)
{
	FVector2D LookAxisVector = value.Get<FVector2D>();
	if (!ControlledPawn) {
		ControlledPawn = GetPawn();
	}
	else {
		ControlledPawn->AddControllerYawInput(LookAxisVector.X);
		ControlledPawn->AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ACh_PlayerController::StopSprint(const FInputActionValue& value)
{
	if (!ControlledPawn) {
		ControlledPawn = GetPawn();
	}
	else {
		ACharacter* MyCharacter = Cast<ACharacter>(ControlledPawn);
		if (MyCharacter) {
			MyCharacter->GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;

		}
	}
}

void ACh_PlayerController::Jump(const FInputActionValue& value)
{
	if (!ControlledPawn) {
		ControlledPawn = GetPawn();
	}
	ACharacter* MyCharacter = Cast<ACharacter>(ControlledPawn);

	if (MyCharacter) {
		MyCharacter->Jump();
	}

}

void ACh_PlayerController::EscapeGame(const FInputActionValue& value)
{
	UWorld* World = GetWorld();
	if (World) {
		APlayerController* PlayerController = World->GetFirstPlayerController();
		UKismetSystemLibrary::QuitGame(World, PlayerController, EQuitPreference::Quit, true);
	}
}


void ACh_PlayerController::Attack(const FInputActionValue& value)
{
	//공격 패킷 전송.
	if (!ControlledPawn) {
		ControlledPawn = GetPawn();
	}
	UPacketExchangeComponent* PacketExchange = nullptr;
	if (ControlledPawn) {
		PacketExchange = Cast<UPacketExchangeComponent>(ControlledPawn->GetComponentByClass(UPacketExchangeComponent::StaticClass()));
		PacketExchange->SendAttackPacket(m_id);
	}
}