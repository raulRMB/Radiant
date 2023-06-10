// Fill out your copyright notice in the Description page of Project Settings.


#include "Avatar.h"
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
AAvatar::AAvatar()
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

void AAvatar::GameReadyUnicast_Implementation()
{
	AbilitySystemComponent->AbilityFailedCallbacks.AddUObject(this, &AAvatar::OnAbilityFailed);
	AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("States.Casting")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAvatar::CastingTagChanged);
	GetController<ARadiantPlayerController>()->GetHUD<ARTHUD>()->HideLoadScreen();
}

void AAvatar::OnAbilityFailed(const UGameplayAbility* GameplayAbility, const FGameplayTagContainer& GameplayTags)
{
	FGameplayTagContainer OwnedTags;
	AbilitySystemComponent->GetOwnedGameplayTags(OwnedTags);
	if (OwnedTags.HasTag(FGameplayTag::RequestGameplayTag(FName("States.Casting"))))
	{
		bShouldActivateBuffer = true;
	} else if(!GameplayAbility->GetName().Contains("BasicAttack"))
	{
		UGameplayStatics::PlaySound2D(GetWorld(), FailedSound, 0.5, 1.5); 
	}
}

void AAvatar::CastingTagChanged(FGameplayTag GameplayTag, int I)
{
	if(bShouldActivateBuffer && I == 0)
	{
		AbilitySystemComponent->HandleGameplayEvent(BufferAbility.EventTag, &BufferAbility);
		bShouldActivateBuffer = false;
	}
}

void AAvatar::GameEnding_Implementation(bool Won)
{
	GetController<ARadiantPlayerController>()->GetHUD<ARTHUD>()->ShowEndScreen(Won);
	UGameplayStatics::PlaySound2D(GetWorld(), Won ? WinSound : LoseSound); 
}

// Called when the game starts or when spawned
void AAvatar::BeginPlay()
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
		AbilitySystemComponent->AbilityFailedCallbacks.AddUObject(this, &AAvatar::OnAbilityFailed);
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("States.Casting")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAvatar::CastingTagChanged);
	}
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &AAvatar::SetFPS, 0.3f, true);
}

void AAvatar::GameReady_Implementation()
{
	SetOwnHealthBarColor();
}

FVector2D AAvatar::GetMousePosition()
{
	FVector2D MousePosition;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetMousePosition(MousePosition);
	}
	return MousePosition;
}

FHitResult AAvatar::GetMousePositionInWorld() const
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

void AAvatar::OnUpdateTarget(const FInputActionValue& Value)
{
	FHitResult HitResult = GetMousePositionInWorld();
	auto Hero = Cast<AAvatar>(HitResult.GetActor());
	if(Hero && Hero->GetPlayerState<ARTPlayerState>()->GetTeamId() != GetPlayerState<ARTPlayerState>()->GetTeamId())
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

void AAvatar::CheckShouldAttack()
{
	if(!Target)
	{
		bIsAttacking = false;
		return;
	}
	
	FVector dir = Target->GetActorLocation() - GetActorLocation();
	bIsAttacking = dir.Size() < AttackRange;

	FGameplayTagContainer OwnedTags;			
	Target->GetAbilitySystemComponent()->GetOwnedGameplayTags(OwnedTags);
	if(OwnedTags.HasTag(FGameplayTag::RequestGameplayTag(FName("States.Dead"))))
	{
		Target = nullptr;
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.BasicAttack")));
		GetAbilitySystemComponent()->CancelAbilities(&TagContainer);
	}
	
	if(IsLocallyControlled() && bIsAttacking)
	{
		FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName("Ability.BasicAttack"));
		CastAbility(Tag);
	}
}

void AAvatar::CastAbility(FGameplayTag& AbilityTag)
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

void AAvatar::OnAbilityOne(const FInputActionValue& Value)
{
	CastAbility(AbilityTags[0]);
}

void AAvatar::OnAbilityTwo(const FInputActionValue& Value)
{
	CastAbility(AbilityTags[1]);
}

void AAvatar::OnAbilityThree(const FInputActionValue& Value)
{
	CastAbility(AbilityTags[2]);
}

void AAvatar::OnAbilityFour(const FInputActionValue& Value)
{
	CastAbility(AbilityTags[3]);
}

void AAvatar::OnAbilityFive(const FInputActionValue& Value)
{
	CastAbility(AbilityTags[4]);
}

void AAvatar::OnAbilitySix(const FInputActionValue& Value)
{
	CastAbility(AbilityTags[5]);
}

void AAvatar::PossessedBy(AController* NewController)
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

void AAvatar::ApplyInitialEffects()
{
	for(auto Effect : InitialEffects)
	{
		UGameplayEffect *EffectInstance = NewObject<UGameplayEffect>(GetTransientPackage(), Effect);
		AbilitySystemComponent->ApplyGameplayEffectToSelf(EffectInstance, 1, AbilitySystemComponent->MakeEffectContext());
	}
}

void AAvatar::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	ARTPlayerState* PS = GetPlayerState<ARTPlayerState>();
	if(PS)
	{
		AbilitySystemComponent = Cast<URTAbilitySystemComponent>(PS->GetAbilitySystemComponent());
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
		
		AttributeSetBase = PS->GetAttributeSetBase();
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AAvatar::OnHealthChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetManaAttribute()).AddUObject(this, &AAvatar::OnManaChanged);
	}
}

void AAvatar::GiveInitialAbilities()
{
	for(auto Ability : Abilities)
	{
		AbilitySystemComponent->GiveAbility(Ability);
	}
}

void AAvatar::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if(Data.Attribute == AttributeSetBase->GetHealthAttribute())
	{
		float Percent = AttributeSetBase->GetHealth() / AttributeSetBase->GetMaxHealth();
		OverHeadInfoBar->SetHealthPercent(Percent);
	}
}

void AAvatar::OnManaChanged(const FOnAttributeChangeData& Data)
{
	if(Data.Attribute == AttributeSetBase->GetManaAttribute())
	{
		float Percent = AttributeSetBase->GetMana() / AttributeSetBase->GetMaxMana();
		OverHeadInfoBar->SetManaPercent(Percent);
	}
}

void AAvatar::SetRotationLock(bool RotationLocked, FVector TargetDir)
{
	bRotationLocked = RotationLocked;
	TargetDirection = TargetDir;
	GetCharacterMovement()->bOrientRotationToMovement = !RotationLocked;
	bUseControllerRotationYaw = RotationLocked;
	S_SetRotationLock(RotationLocked, TargetDir);
}

void AAvatar::ResetDestination()
{
	Destination = GetActorLocation();
}

void AAvatar::C_ResetDestination_Implementation()
{
	ResetDestination();
}

void AAvatar::SetDestination(FVector NewDestination)
{
	Destination = NewDestination;
}

void AAvatar::M_SetInfoBarVisibility_Implementation(bool bVisible)
{
	OverHeadInfoBarWidgetComponent->SetVisibility(bVisible);
}

void AAvatar::SetOwnHealthBarColor()
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

void AAvatar::SetHealthColor(const FLinearColor Color)
{
	if(OverHeadInfoBar)
	{
		OverHeadInfoBar->SetHealthColor(Color);
	}
}

void AAvatar::SetAllHealthBarColors()
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
							State->GetPawn<AAvatar>()->SetHealthColor(FLinearColor::Green);
						}
						else
						{
							State->GetPawn<AAvatar>()->SetHealthColor(FLinearColor::Red);
						}
					}
				}
			}
		}
	}
}

FVector AAvatar::GetHalfHeightVector()
{
	return FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
}

bool AAvatar::HasTag(FString Tag)
{
	FGameplayTagContainer TagContainer;
	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->GetOwnedGameplayTags(TagContainer);
		return TagContainer.HasTag(FGameplayTag::RequestGameplayTag(FName(Tag)));
	}
	return false;
}

void AAvatar::ToggleCameraBool(const FInputActionValue& Value)
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

void AAvatar::HoldCamera(const FInputActionValue& Value)
{
	if(!bCameraLocked)
	{
		MainCamera->SetActive(true);
		UnlockedCamera->SetActive(false);
		bCameraHeld = true;
	}
}

void AAvatar::ReleaseHoldCamera(const FInputActionValue& InputActionValue)
{
	if(!bCameraLocked)
	{
		MainCamera->SetActive(false);
		UnlockedCamera->SetActive(true);
		UnlockedCamera->SetWorldLocation(MainCamera->GetComponentLocation());
		bCameraHeld = false;
	}
}

void AAvatar::AttackMove(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Attack Move"));
	const FHitResult HitResult = GetMousePositionInWorld();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, AttackMoveSystemTemplate, HitResult.Location);
}

void AAvatar::HandleCamera()
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

void AAvatar::Restart()
{
	Super::Restart();
}

void AAvatar::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AAvatar::PostLoad()
{
	Super::PostLoad();
}

void AAvatar::S_SetRotationLock_Implementation(bool RotationLocked, FVector TargetDir)
{
	bRotationLocked = RotationLocked;
	TargetDirection = TargetDir;
	GetCharacterMovement()->bOrientRotationToMovement = !RotationLocked;
	bUseControllerRotationYaw = RotationLocked;
}

void AAvatar::SetFPS()
{
	if(GetLocalRole() == ROLE_AutonomousProxy)
	{
		ARadiantPlayerController* PC = GetController<ARadiantPlayerController>();
		if(PC)
		{
			PC->GetHUD<ARTHUD>()->SetFPS(FPS);
		}
	}
}

// Called every frame
void AAvatar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FPS = 1.0/DeltaTime;
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

void AAvatar::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// Called to bind functionality to input
void AAvatar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &AAvatar::OnUpdateTarget);
		EnhancedInputComponent->BindAction(AbilityOneAction, ETriggerEvent::Started, this, &AAvatar::OnAbilityOne);
		EnhancedInputComponent->BindAction(AbilityTwoAction, ETriggerEvent::Started, this, &AAvatar::OnAbilityTwo);
		EnhancedInputComponent->BindAction(AbilityThreeAction, ETriggerEvent::Started, this, &AAvatar::OnAbilityThree);
		EnhancedInputComponent->BindAction(AbilityFourAction, ETriggerEvent::Started, this, &AAvatar::OnAbilityFour);
		EnhancedInputComponent->BindAction(AbilityFiveAction, ETriggerEvent::Started, this, &AAvatar::OnAbilityFive);
		EnhancedInputComponent->BindAction(AbilitySixAction, ETriggerEvent::Started, this, &AAvatar::OnAbilitySix);
		EnhancedInputComponent->BindAction(CameraToggleAction, ETriggerEvent::Started, this, &AAvatar::ToggleCameraBool);
		EnhancedInputComponent->BindAction(CameraHoldAction, ETriggerEvent::Started, this, &AAvatar::HoldCamera);
		EnhancedInputComponent->BindAction(CameraHoldAction, ETriggerEvent::Completed, this, &AAvatar::ReleaseHoldCamera);
		EnhancedInputComponent->BindAction(AttackMoveAction, ETriggerEvent::Started, this, &AAvatar::AttackMove);
	}
}

