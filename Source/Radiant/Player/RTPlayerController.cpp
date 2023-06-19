// Copyright Radiant Studios

#include "RTPlayerController.h"

#include "Avatar.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Modes/Base/RTGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Util/Util.h"

void ARTPlayerController::PlayerLoaded_Implementation()
{
	ARTGameMode* GM = GetWorld()->GetAuthGameMode<ARTGameMode>();
	GM->PlayerLoaded();
}

void ARTPlayerController::S_SetPlayerStart_Implementation(ARTPlayerStart* NewPlayerStart)
{
	PlayerStart = NewPlayerStart;	
}

void ARTPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARTPlayerController, PlayerStart)
}

void ARTPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(MappingContext, 0);
	}
}

void ARTPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &ARTPlayerController::OnClick);
		EnhancedInputComponent->BindAction(AbilityOneAction, ETriggerEvent::Started, this, &ARTPlayerController::OnAbilityOne);
		EnhancedInputComponent->BindAction(AbilityTwoAction, ETriggerEvent::Started, this, &ARTPlayerController::OnAbilityTwo);
		EnhancedInputComponent->BindAction(AbilityThreeAction, ETriggerEvent::Started, this, &ARTPlayerController::OnAbilityThree);
		EnhancedInputComponent->BindAction(AbilityFourAction, ETriggerEvent::Started, this, &ARTPlayerController::OnAbilityFour);
		EnhancedInputComponent->BindAction(AbilityFiveAction, ETriggerEvent::Started, this, &ARTPlayerController::OnAbilityFive);
		EnhancedInputComponent->BindAction(AbilitySixAction, ETriggerEvent::Started, this, &ARTPlayerController::OnAbilitySix);
		EnhancedInputComponent->BindAction(CameraToggleAction, ETriggerEvent::Started, this, &ARTPlayerController::ToggleCameraBool);
		EnhancedInputComponent->BindAction(CameraHoldAction, ETriggerEvent::Started, this, &ARTPlayerController::HoldCamera);
		EnhancedInputComponent->BindAction(CameraHoldAction, ETriggerEvent::Completed, this, &ARTPlayerController::ReleaseHoldCamera);
		EnhancedInputComponent->BindAction(AttackMoveAction, ETriggerEvent::Started, this, &ARTPlayerController::AttackMove);
	}
}

void ARTPlayerController::OnClick(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->OnUpdateTarget(Value);
}

void ARTPlayerController::OnAbilityOne(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->OnAbilityOne(Value);
}

void ARTPlayerController::OnAbilityTwo(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->OnAbilityTwo(Value);
}

void ARTPlayerController::OnAbilityThree(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->OnAbilityThree(Value);
}

void ARTPlayerController::OnAbilityFour(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->OnAbilityFour(Value);
}

void ARTPlayerController::OnAbilityFive(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->OnAbilityFive(Value);
}

void ARTPlayerController::OnAbilitySix(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->OnAbilitySix(Value);
}

void ARTPlayerController::ToggleCameraBool(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->ToggleCameraBool(Value);
}

void ARTPlayerController::HoldCamera(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->HoldCamera(Value);
}

void ARTPlayerController::ReleaseHoldCamera(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->ReleaseHoldCamera(Value);
}

void ARTPlayerController::AttackMove(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->AttackMove(Value);
}
