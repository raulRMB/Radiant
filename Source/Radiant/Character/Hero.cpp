// Fill out your copyright notice in the Description page of Project Settings.


#include "Hero.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InterchangeResult.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AI/NavigationSystemBase.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "GAS/AttributeSets/RTHeroAttributeSetBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/RTPlayerState.h"
#include "UI/HeroInfoBar.h"
#include "Util/Util.h"
#include "Util/MouseVec.h"

// Sets default values
AHero::AHero()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bCameraLocked = true;

	MainCamera = CreateDefaultSubobject<UCameraComponent>(FName("MainCamera"));
	UnlockedCamera = CreateDefaultSubobject<UCameraComponent>(FName("UnlockedCamera"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(FName("SpringArm"));

	SpringArm->SetupAttachment(RootComponent);
	MainCamera->SetupAttachment(SpringArm);
	UnlockedCamera->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	
	HitBox = CreateDefaultSubobject<UCapsuleComponent>(FName("HitBox"));
	check(HitBox);
	HitBox->SetupAttachment(RootComponent);
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	OverHeadInfoBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("InfoBar");
	OverHeadInfoBarWidgetComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AHero::BeginPlay()
{
	Super::BeginPlay();

	Destination = GetActorLocation();
	UnlockedCamera->SetWorldRotation(MainCamera->GetComponentRotation());
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		PlayerController->bShowMouseCursor = true;
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}
	
	OverHeadInfoBar = Cast<UHeroInfoBar>(OverHeadInfoBarWidgetComponent->GetWidget());
	if(OverHeadInfoBar)
	{
		OverHeadInfoBar->SetHealthPercent(1.f);
		OverHeadInfoBar->SetManaPercent(1.f);
	}
}

FVector2D AHero::GetMousePosition()
{
	FVector2D MousePosition;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetMousePosition(MousePosition);
	}
	return MousePosition;
}

FHitResult AHero::GetMousePositionInWorld() const
{
	FVector2D MousePosition = GetMousePosition();
	FVector WorldPosition;
	FVector WorldDirection;
	UGameplayStatics::DeprojectScreenToWorld(GetWorld()->GetFirstPlayerController(), MousePosition, WorldPosition, WorldDirection);
	FHitResult HitResult;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(HitResult, WorldPosition, WorldPosition + WorldDirection * 1000000, ECC_GameTraceChannel1, CollisionQueryParams);
	return HitResult;
}

void AHero::OnUpdateTarget(const FInputActionValue& Value)
{
	FHitResult HitResult = GetMousePositionInWorld();
	if(auto Hero = Cast<AHero>(HitResult.GetActor()))
	{
		Target = Hero;
		FGameplayEventData EventData;
		EventData.OptionalObject = Target;
		EventData.Instigator = this;
		const FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Ability.BasicAttack.Ranged"));
		AbilitySystemComponent->HandleGameplayEvent(EventTag, &EventData);
	}
	else
	{
		Target = nullptr;
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, SystemTemplate, HitResult.Location);
	}
	Destination = HitResult.Location;
}

void AHero::CheckShouldAttack()
{
	if(!Target)
	{
		bIsAttacking = false;
		return;
	}
	
	FVector dir = Target->GetActorLocation() - GetActorLocation();
	bIsAttacking = dir.Size() < AttackRange;
}

void AHero::OnAbilityOne(const FInputActionValue& Value)
{
	TArray<AActor*> Actors;
	const FVector MousePos = UUtil::GetMousePosition(GetWorld(), Actors);
	FGameplayEventData EventData;
	const UMouseVec* dir = NewObject<UMouseVec>();
	FVector Loc = GetActorLocation();
	Loc.Z = 0;
	dir->MouseVec = UUtil::ProjectileDirection(Loc, MousePos);
	EventData.OptionalObject = dir;
	EventData.Instigator = this;
	const FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Ability.Combat.Fireball"));
	AbilitySystemComponent->HandleGameplayEvent(EventTag, &EventData);
}

void AHero::OnAbilityTwo(const FInputActionValue& Value)
{
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Status.Buff.Damage")));
	AbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);
}

void AHero::OnAbilityThree(const FInputActionValue& Value)
{
	TArray<AActor*> Actors;
	const FVector MousePos = UUtil::GetMousePosition(GetWorld(), Actors);
	FGameplayEventData EventData;
	const UMouseVec* MouseData = NewObject<UMouseVec>();
	MouseData->MouseVec = MousePos;
	EventData.OptionalObject = MouseData;
	EventData.Instigator = this;
	const FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Ability.Movement.Dash"));
	AbilitySystemComponent->HandleGameplayEvent(EventTag, &EventData);
}

void AHero::OnAbilityFour(const FInputActionValue& Value)
{
	TArray<AActor*> Actors;
	const FVector MousePos = UUtil::GetMousePosition(GetWorld(), Actors);
	FGameplayEventData EventData;
	const UMouseVec* MouseData = NewObject<UMouseVec>();
	MouseData->MouseVec = MousePos;
	EventData.OptionalObject = MouseData;
	EventData.Instigator = this;
	const FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Ability.Cast.VoidImplosion"));
	AbilitySystemComponent->HandleGameplayEvent(EventTag, &EventData);
}

void AHero::OnAbilityFive(const FInputActionValue& Value)
{
	
}

void AHero::OnAbilitySix(const FInputActionValue& Value)
{
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Heal")));
	AbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);
}

void AHero::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ARTPlayerState* PS = GetPlayerState<ARTPlayerState>();
	if(PS)
	{
		AbilitySystemComponent = Cast<URTAbilitySystemComponent>(PS->GetAbilitySystemComponent());
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
		
		AttributeSetBase = PS->GetAttributeSetBase();
	}

	GiveInitialAbilities();
}

void AHero::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	ARTPlayerState* PS = GetPlayerState<ARTPlayerState>();
	if(PS)
	{
		AbilitySystemComponent = Cast<URTAbilitySystemComponent>(PS->GetAbilitySystemComponent());
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
		
		AttributeSetBase = PS->GetAttributeSetBase();
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AHero::OnHealthChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetManaAttribute()).AddUObject(this, &AHero::OnManaChanged);
	}
}

void AHero::GiveInitialAbilities()
{
	for(auto Ability : Abilities)
	{
		AbilitySystemComponent->GiveAbility(Ability);
	}
}

void AHero::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if(Data.Attribute == AttributeSetBase->GetHealthAttribute())
	{
		float Percent = AttributeSetBase->GetHealth() / AttributeSetBase->GetMaxHealth();
		OverHeadInfoBar->SetHealthPercent(Percent);
	}
}

void AHero::OnManaChanged(const FOnAttributeChangeData& Data)
{
	if(Data.Attribute == AttributeSetBase->GetManaAttribute())
	{
		float Percent = AttributeSetBase->GetMana() / AttributeSetBase->GetMaxMana();
		OverHeadInfoBar->SetManaPercent(Percent);
	}
}

void AHero::SetRotationLock(bool RotationLocked, FVector TargetDir)
{
	bRotationLocked = RotationLocked;
	TargetDirection = TargetDir;
	GetCharacterMovement()->bOrientRotationToMovement = !RotationLocked;
	bUseControllerRotationYaw = RotationLocked;
	S_SetRotationLock(RotationLocked, TargetDir);
}

void AHero::ResetDestination()
{
	Destination = GetActorLocation();
}

void AHero::SetDestination(FVector NewDestination)
{
	Destination = NewDestination;
}

bool AHero::HasTag(FString Tag)
{
	FGameplayTagContainer TagContainer;
	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->GetOwnedGameplayTags(TagContainer);
		return TagContainer.HasTag(FGameplayTag::RequestGameplayTag(FName(Tag)));
	}
	return false;
}

void AHero::ToggleCameraBool(const FInputActionValue& Value)
{
	bCameraLocked = !bCameraLocked;
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(bCameraLocked ? EMouseLockMode::DoNotLock : EMouseLockMode::LockAlways);
	GetController<APlayerController>()->SetInputMode(InputMode);
	GetController<APlayerController>()->SetInputMode(InputMode);
	UnlockedCamera->SetWorldLocation(MainCamera->GetComponentLocation());
	UnlockedCamera->SetActive(!bCameraLocked);
	MainCamera->SetActive(bCameraLocked);
}

void AHero::HoldCamera(const FInputActionValue& Value)
{
	if(!bCameraLocked)
	{
		MainCamera->SetActive(true);
		UnlockedCamera->SetActive(false);
		bCameraHeld = true;
	}
}

void AHero::ReleaseHoldCamera(const FInputActionValue& InputActionValue)
{
	if(!bCameraLocked)
	{
		MainCamera->SetActive(false);
		UnlockedCamera->SetActive(true);
		UnlockedCamera->SetWorldLocation(MainCamera->GetComponentLocation());
		bCameraHeld = false;
	}
}

void AHero::AttackMove(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Attack Move"));
	const FHitResult HitResult = GetMousePositionInWorld();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, AttackMoveSystemTemplate, HitResult.Location);
}

void AHero::HandleCamera()
{
	if(!bCameraLocked)
	{
		FVector2D MousePosition = GetMousePosition();
		FVector2D ViewportSize;
		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(ViewportSize);
		}

		FVector CameraMove = FVector::ZeroVector;
		if(MousePosition.X <= CameraMovementThreshold)
		{
			CameraMove += FVector::ForwardVector * -CameraMovementSpeed;
		} else if(MousePosition.X >= ViewportSize.X - CameraMovementThreshold)
		{
			CameraMove += FVector::ForwardVector * CameraMovementSpeed;
		}

		if(MousePosition.Y <= CameraMovementThreshold)
		{
			CameraMove += FVector::RightVector * -CameraMovementSpeed;
		} else if(MousePosition.Y >= ViewportSize.Y - CameraMovementThreshold)
		{
			CameraMove += FVector::RightVector * CameraMovementSpeed;
		}
		CameraMove = CameraMove.RotateAngleAxis(45, FVector::UpVector);
		UnlockedCamera->SetWorldLocation(UnlockedCamera->GetComponentLocation() + CameraMove);
	}
}

void AHero::S_SetRotationLock_Implementation(bool RotationLocked, FVector TargetDir)
{
	bRotationLocked = RotationLocked;
	TargetDirection = TargetDir;
	GetCharacterMovement()->bOrientRotationToMovement = !RotationLocked;
	bUseControllerRotationYaw = RotationLocked;
}

// Called every frame
void AHero::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HandleCamera();
	CheckShouldAttack();
	
	if(!HasTag("States.Movement.Stopped") && !bIsAttacking)
	{
		if((Destination - GetActorLocation()).Size() >= 200.f)
		{
			FVector dir = Destination - GetActorLocation();
			AddMovementInput(dir, 1);
		}
	}
	if(IsLocallyControlled() && bRotationLocked)
	{
		FRotator CurrentRotation = GetCapsuleComponent()->GetComponentRotation();
		FRotator Rotation = FMath::Lerp(CurrentRotation, TargetDirection.Rotation(), RotationSpeed * DeltaTime);
		Rotation.Pitch = CurrentRotation.Pitch;
		Rotation.Roll = CurrentRotation.Roll;
		GetController()->SetControlRotation(Rotation);
	}
}

void AHero::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// Called to bind functionality to input
void AHero::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &AHero::OnUpdateTarget);
		EnhancedInputComponent->BindAction(AbilityOneAction, ETriggerEvent::Started, this, &AHero::OnAbilityOne);
		EnhancedInputComponent->BindAction(AbilityTwoAction, ETriggerEvent::Started, this, &AHero::OnAbilityTwo);
		EnhancedInputComponent->BindAction(AbilityThreeAction, ETriggerEvent::Started, this, &AHero::OnAbilityThree);
		EnhancedInputComponent->BindAction(AbilityFourAction, ETriggerEvent::Started, this, &AHero::OnAbilityFour);
		EnhancedInputComponent->BindAction(AbilityFiveAction, ETriggerEvent::Started, this, &AHero::OnAbilityFive);
		EnhancedInputComponent->BindAction(AbilitySixAction, ETriggerEvent::Started, this, &AHero::OnAbilitySix);
		EnhancedInputComponent->BindAction(CameraToggleAction, ETriggerEvent::Started, this, &AHero::ToggleCameraBool);
		EnhancedInputComponent->BindAction(CameraHoldAction, ETriggerEvent::Started, this, &AHero::HoldCamera);
		EnhancedInputComponent->BindAction(CameraHoldAction, ETriggerEvent::Completed, this, &AHero::ReleaseHoldCamera);
		EnhancedInputComponent->BindAction(AttackMoveAction, ETriggerEvent::Started, this, &AHero::AttackMove);
	}
}

