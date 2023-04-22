// Fill out your copyright notice in the Description page of Project Settings.


#include "Hero.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AHero::AHero()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HitBox = CreateDefaultSubobject<UCapsuleComponent>(FName("HitBox"));
	check(HitBox);
	HitBox->SetupAttachment(RootComponent);
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
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
	GetWorld()->LineTraceSingleByChannel(HitResult, WorldPosition, WorldPosition + WorldDirection * 1000000, ECC_GameTraceChannel1);
	
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

	bCanRotate = true;
	
	// Set the destination
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

void AHero::TurnToDestination(float DeltaTime)
{
	if (!bCanRotate)
		return;

	FVector Dir = (Destination - GetActorLocation()).GetSafeNormal();
	float Angle = FMath::RadiansToDegrees(FMath::Atan2(Dir.Y, Dir.X));

	FRotator NewRotation = FRotator(0, Angle, 0);
	
	SetActorRotation(NewRotation);
}

// Called every frame
void AHero::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckShouldAttack();

	TurnToDestination(DeltaTime);
	
	if(!bIsAttacking)
	{
		if((Destination - GetActorLocation()).Size() < 10)
			return;
		FVector dir = Destination - GetActorLocation();
		AddMovementInput(dir, 1);
	}
}

// Called to bind functionality to input
void AHero::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &AHero::OnUpdateTarget);
	}
}

