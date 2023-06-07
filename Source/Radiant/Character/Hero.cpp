// Fill out your copyright notice in the Description page of Project Settings.


#include "Hero.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "RadiantPlayerController.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"
#include "AI/NavigationSystemBase.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAS/Abilities/RTAbility.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "GAS/AttributeSets/RTHeroAttributeSetBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/RTPlayerState.h"
#include "Modes/RTGameState.h"
#include "UI/HeroInfoBar.h"
#include "UI/RTHUD.h"
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

void AHero::GameReadyUnicast_Implementation()
{
	GetController<ARadiantPlayerController>()->GetHUD<ARTHUD>()->HideLoadScreen();
}

void AHero::OnAbilityFailed(const UGameplayAbility* GameplayAbility, const FGameplayTagContainer& GameplayTags)
{
	FGameplayTagContainer OwnedTags;
	AbilitySystemComponent->GetOwnedGameplayTags(OwnedTags);
	if (OwnedTags.HasTag(FGameplayTag::RequestGameplayTag(FName("States.Casting"))))
	{
		bShouldActivateBuffer = true;
	}
}

void AHero::CastingTagChanged(FGameplayTag GameplayTag, int I)
{
	if(bShouldActivateBuffer && I == 0)
	{
		AbilitySystemComponent->HandleGameplayEvent(BufferAbility.EventTag, &BufferAbility);
		bShouldActivateBuffer = false;
	}
}

void AHero::GameEnding_Implementation(bool Won)
{
	GetController<ARadiantPlayerController>()->GetHUD<ARTHUD>()->ShowEndScreen(Won);
	UGameplayStatics::PlaySound2D(GetWorld(), Won ? WinSound : LoseSound); 
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
	if(!HasAuthority())
	{
		auto PC = Cast<ARadiantPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		PC->PlayerLoaded();
	}
	SetOwnHealthBarColor();
	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityFailedCallbacks.AddUObject(this, &AHero::OnAbilityFailed);
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("States.Casting")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AHero::CastingTagChanged);
	}

}

void AHero::GameReady_Implementation()
{
	ApplyInitialEffects();
	SetOwnHealthBarColor();
	AbilitySystemComponent->AbilityFailedCallbacks.AddUObject(this, &AHero::OnAbilityFailed);
	AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("States.Casting")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AHero::CastingTagChanged);
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

	FHitResult PlayerHitResult;
	FHitResult GroundHitResult;
	FCollisionQueryParams CollisionQueryParams;
	GetWorld()->LineTraceSingleByChannel(PlayerHitResult, WorldPosition, WorldPosition + WorldDirection * 1000000, ECC_GameTraceChannel1, CollisionQueryParams);
	GetWorld()->LineTraceSingleByChannel(GroundHitResult, WorldPosition, WorldPosition + WorldDirection * 1000000, ECC_GameTraceChannel2, CollisionQueryParams);
	
	return FHitResult(PlayerHitResult.GetActor(), PlayerHitResult.GetComponent(), GroundHitResult.Location, GroundHitResult.ImpactNormal);
}

void AHero::OnUpdateTarget(const FInputActionValue& Value)
{
	FHitResult HitResult = GetMousePositionInWorld();
	if(auto Hero = Cast<AHero>(HitResult.GetActor()))
	{
		Target = Hero;
		GetPlayerState<ARTPlayerState>()->S_SetTargetId(Hero->GetPlayerState<ARTPlayerState>()->GetPlayerId());
	}
	else
	{
		Target = nullptr;
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, SystemTemplate, HitResult.Location);
	}
	Destination = HitResult.Location;
	if((Destination - GetActorLocation()).Size() >= 200)
	{
		bAtDestination = false;
	}
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
	
	if(IsLocallyControlled() && bIsAttacking)
	{
		FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName("Ability.BasicAttack"));
		CastAbility(Tag);
	}
}

void AHero::CastAbility(FGameplayTag& AbilityTag)
{
	TArray<AActor*> Actors;
	FGameplayEventData EventData;
	FGameplayAbilityTargetData_SingleTargetHit* MousePosData = new FGameplayAbilityTargetData_SingleTargetHit();

	FHitResult HitResult = GetMousePositionInWorld();
	MousePosData->HitResult = HitResult;
	
	FGameplayAbilityTargetDataHandle TargetData;
	TargetData.Add(MousePosData);
	
	EventData.TargetData = TargetData;
	EventData.EventTag = AbilityTag;
	const FGameplayTag EventTag = AbilityTag;

	BufferAbility = EventData;
	
	AbilitySystemComponent->HandleGameplayEvent(EventTag, &EventData);
}

void AHero::OnAbilityOne(const FInputActionValue& Value)
{
	CastAbility(AbilityTags[0]);
}

void AHero::OnAbilityTwo(const FInputActionValue& Value)
{
	CastAbility(AbilityTags[1]);
}

void AHero::OnAbilityThree(const FInputActionValue& Value)
{
	CastAbility(AbilityTags[2]);
}

void AHero::OnAbilityFour(const FInputActionValue& Value)
{
	CastAbility(AbilityTags[3]);
}

void AHero::OnAbilityFive(const FInputActionValue& Value)
{
	CastAbility(AbilityTags[4]);
}

void AHero::OnAbilitySix(const FInputActionValue& Value)
{
	CastAbility(AbilityTags[5]);
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

void AHero::ApplyInitialEffects()
{
	for(auto Effect : InitialEffects)
	{
		UGameplayEffect *EffectInstance = NewObject<UGameplayEffect>(GetTransientPackage(), Effect);
		AbilitySystemComponent->ApplyGameplayEffectToSelf(EffectInstance, 1, AbilitySystemComponent->MakeEffectContext());
	}
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

void AHero::C_ResetDestination_Implementation()
{
	ResetDestination();
}

void AHero::SetDestination(FVector NewDestination)
{
	Destination = NewDestination;
}

void AHero::M_SetInfoBarVisibility_Implementation(bool bVisible)
{
	OverHeadInfoBarWidgetComponent->SetVisibility(bVisible);
}

void AHero::SetOwnHealthBarColor()
{
	if(GetPlayerState<ARTPlayerState>())
	{
		APlayerController *PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		ARTPlayerState* LocalPS = Cast<ARTPlayerState>(PC->GetPlayerState<ARTPlayerState>());
		if(!LocalPS)
		{
			return;
		}
		int Id = GetPlayerState<ARTPlayerState>()->TeamId;
		FLinearColor Color;
		if(LocalPS)
		{
			if(Id == LocalPS->TeamId)
			{
				Color = FLinearColor::Green;
			}
			else
			{
				Color = FLinearColor::Red;
			}
		}
		SetHealthColor(Color);
	}
}

void AHero::SetHealthColor(const FLinearColor Color)
{
	if(OverHeadInfoBar)
	{
		OverHeadInfoBar->SetHealthColor(Color);
	}
}

void AHero::SetAllHealthBarColors()
{
	if(GetLocalRole() == ROLE_AutonomousProxy)
	{
		if(GetPlayerState<ARTPlayerState>())
		{
			int Id = GetPlayerState<ARTPlayerState>()->TeamId;
			TArray<APlayerState*> States = GetWorld()->GetGameState<ARTGameState>()->PlayerArray;
			if(States.Num() > 0)
			{
				for(auto State : States)
				{
					if(auto s = Cast<ARTPlayerState>(State))
					{
						int OtherId = s->TeamId;
						if(Id == OtherId)
						{
							State->GetPawn<AHero>()->SetHealthColor(FLinearColor::Green);
						}
						else
						{
							State->GetPawn<AHero>()->SetHealthColor(FLinearColor::Red);
						}
					}
				}
			}
		}
	}
}

FVector AHero::GetHalfHeightVector()
{
	return FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
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
		CameraMove = CameraMove.RotateAngleAxis(UnlockedCamera->GetComponentRotation().Yaw + 90, FVector::UpVector);
		UnlockedCamera->SetWorldLocation(UnlockedCamera->GetComponentLocation() + CameraMove);
	}
}

void AHero::Restart()
{
	Super::Restart();
}

void AHero::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AHero::PostLoad()
{
	Super::PostLoad();
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
		if(Target)
		{
			Destination = Target->GetActorLocation();
		}
		if(!bAtDestination && (Destination - GetActorLocation()).Size() >= 200.f)
		{
			FVector dir = Destination - GetActorLocation();
			AddMovementInput(dir, 1);
		}
		else
		{
			bAtDestination = true;
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

