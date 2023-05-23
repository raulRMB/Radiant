// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "GAS/AttributeSets/RTHeroAttributeSetBase.h"
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

	UPROPERTY(EditAnywhere)
	float RotationSpeed = 20.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	uint8 bRotationLocked : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	uint8 bCameraLocked : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	uint8 bCameraHeld : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector TargetDirection;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRange;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class UCapsuleComponent* HitBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	class UCameraComponent* UnlockedCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	class UCameraComponent* MainCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* SpringArm;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	class UInputAction* ClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	class UInputAction* AttackMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	class UInputAction* CameraToggleAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	class UInputAction* CameraHoldAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	class UInputAction* AbilityOneAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	class UInputAction* AbilityTwoAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	class UInputAction* AbilityThreeAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	class UInputAction* AbilityFourAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	class UInputAction* AbilityFiveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	class UInputAction* AbilitySixAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	class UInputMappingContext* MappingContext;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Combat")
	TArray<TSubclassOf<class UGameplayAbility>> Abilities;
	
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* SystemTemplate;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* AttackMoveSystemTemplate;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	static FVector2D GetMousePosition();
	FHitResult GetMousePositionInWorld() const;
	void OnUpdateTarget(const FInputActionValue& Value);
	void CheckShouldAttack();

	UFUNCTION()
	void OnAbilityOne(const FInputActionValue& Value);

	UFUNCTION()
	void OnAbilityTwo(const FInputActionValue& Value);

	UFUNCTION()
	void OnAbilityThree(const FInputActionValue& Value);

	UFUNCTION()
	void OnAbilityFour(const FInputActionValue& Value);

	UFUNCTION()
	void OnAbilityFive(const FInputActionValue& Value);

	UFUNCTION()
	void OnAbilitySix(const FInputActionValue& Value);

	UFUNCTION()
	void ToggleCameraBool(const FInputActionValue& Value);

	UFUNCTION()
	void HoldCamera(const FInputActionValue& Value);

	UFUNCTION()	
	void ReleaseHoldCamera(const FInputActionValue& InputActionValue);

	UFUNCTION()
	void AttackMove(const FInputActionValue& Value);
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	void GiveInitialAbilities();
	
	class URTAbilitySystemComponent* AbilitySystemComponent;
	class URTHeroAttributeSetBase* AttributeSetBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	class UWidgetComponent* OverHeadInfoBarWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraMovementThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraMovementSpeed;
	
	class UHeroInfoBar* OverHeadInfoBar;

	void OnHealthChanged(const FOnAttributeChangeData& Data);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintCallable)
	void SetRotationLock(bool RotationLocked, FVector TargetDir);
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void S_SetRotationLock(bool RotationLocked, FVector TargetDir);

	UFUNCTION(BlueprintCallable)
	void ResetDestination();
	UFUNCTION(BlueprintCallable)
	void SetDestination(FVector NewDestination);
private:
	bool HasTag(FString Tag);
	void HandleCamera();
};
