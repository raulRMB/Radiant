// Fill out your copyright notice in the Description page of Project Settings.


#include "Hero.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
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
		OverHeadInfoBar->SetHealthPercent(1.f);
}

void AHero::OnUpdateTarget(const FInputActionValue& Value)
{
	// Get the mouse position
	FVector2D MousePosition;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetMousePosition(MousePosition);
	}

	// Get the world position
	FVector WorldPosition;
	FVector WorldDirection;
	UGameplayStatics::DeprojectScreenToWorld(GetWorld()->GetFirstPlayerController(), MousePosition, WorldPosition, WorldDirection);

	
	// Get the hit result
	FHitResult HitResult;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(HitResult, WorldPosition, WorldPosition + WorldDirection * 1000000, ECC_GameTraceChannel1, CollisionQueryParams);
	

	if(auto Hero = Cast<AHero>(HitResult.GetActor()))
	{
		Target = Hero;
		UE_LOG(LogTemp, Warning, TEXT("Target: %s"), *Target->GetName());
	}
	else
	{
		Target = nullptr;
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, SystemTemplate, HitResult.Location);
	}

	FGameplayTagContainer TagContainer;
	AbilitySystemComponent->GetOwnedGameplayTags(TagContainer);
	if(TagContainer.HasTag(FGameplayTag::RequestGameplayTag(FName("States.Movement.Stopped"))))
		Destination = GetActorLocation();
	else
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
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Heal")));
	AbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);	
}

void AHero::OnAbilityTwo(const FInputActionValue& Value)
{
	TArray<AActor*> Actors;
	const FVector MousePos = UUtil::GetMousePosition(GetWorld(), Actors);
	FGameplayEventData EventData;
	const UMouseVec* dir = NewObject<UMouseVec>();
	dir->MouseVec = UUtil::ProjectileDirection(GetActorLocation(), MousePos);
	EventData.OptionalObject = dir;
	EventData.Instigator = this;
	const FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Ability.Combat.Fireball"));
	AbilitySystemComponent->HandleGameplayEvent(EventTag, &EventData);
}

void AHero::OnAbilityThree(const FInputActionValue& Value)
{
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Movement.Sprint")));
	AbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);
}

void AHero::OnAbilityFour(const FInputActionValue& Value)
{
	
}

void AHero::OnAbilityFive(const FInputActionValue& Value)
{
	
}

void AHero::OnAbilitySix(const FInputActionValue& Value)
{
	
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
		float Percent = AttributeSetBase->GetHealth() / 100;
		OverHeadInfoBar->SetHealthPercent(Percent);
	}
}

void AHero::StopMovement()
{
	Destination = GetActorLocation();
}

void AHero::SetRotationLock(bool RotationLocked, FVector TargetDir)
{
	bRotationLocked = RotationLocked;
	TargetDirection = TargetDir;
	GetCharacterMovement()->bOrientRotationToMovement = !RotationLocked;
	bUseControllerRotationYaw = RotationLocked;
	S_SetRotationLock(RotationLocked, TargetDir);
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

	CheckShouldAttack();
	
	if(!bIsAttacking)
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
	}
}

