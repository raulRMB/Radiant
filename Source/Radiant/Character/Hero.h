// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Hero.generated.h"

UCLASS()
class RADIANT_API AHero : public ACharacter
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	AHero();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		FVector Destination;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		class AHero* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		uint8 bIsAttacking : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float AttackRange;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
		class UCapsuleComponent* HitBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
		class UInputAction* ClickAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
		class UInputMappingContext* MappingContext;
	
	UPROPERTY(EditAnywhere)
		class UNiagaraSystem* SystemTemplate;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void OnUpdateTarget(const FInputActionValue& Value);
	void CheckShouldAttack();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
