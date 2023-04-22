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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hero")
		FVector Destination;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
		class UInputAction* ClickAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* MappingContext;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void Click(const FInputActionValue& Value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
