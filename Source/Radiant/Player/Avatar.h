// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "GAS/AttributeSets/RTHeroAttributeSetBase.h"
#include "Player/RTPlayerState.h"
#include "Avatar.generated.h"

UCLASS()
class RADIANT_API AAvatar : public ACharacter
{
	GENERATED_BODY()
	
public:
	AAvatar();

	UFUNCTION(NetMulticast, Reliable)
	void GameReady();
	UFUNCTION(Client, Reliable)
	void GameReadyUnicast();
	void OnAbilityFailed(const UGameplayAbility* GameplayAbility, const FGameplayTagContainer& GameplayTags);
	void CastingTagChanged(FGameplayTag GameplayTag, int I);

	UFUNCTION(Client, Reliable)
	void GameEnding(bool Won);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector Destination;
	
	uint8 bAtDestination : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class AAvatar* Target;
	
	UPROPERTY(EditAnywhere)
	class USoundBase* WinSound;

	UPROPERTY(EditAnywhere)
	class USoundBase* LoseSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	uint8 bIsAttacking : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	uint8 bCanAttack : 1;

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

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Combat")
	TArray<TSubclassOf<class UGameplayAbility>> Abilities;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Combat")
	TArray<TSubclassOf<class UGameplayEffect>> InitialEffects;
	
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* SystemTemplate;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* AttackMoveSystemTemplate;

	UPROPERTY(EditAnywhere)
	TArray<FGameplayTag> AbilityTags;

	UPROPERTY(EditAnywhere)
	USoundBase* FailedSound;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	static FVector2D GetMousePosition();
	FHitResult GetMousePositionInWorld() const;
	void CheckShouldAttack();
public:
	void ApplyInitialEffects();
	void CastAbility(FGameplayTag& AbilityTag);
	
	void OnUpdateTarget(const FInputActionValue& Value);
	void OnAbilityOne(const FInputActionValue& Value);
	void OnAbilityTwo(const FInputActionValue& Value);
	void OnAbilityThree(const FInputActionValue& Value);
	void OnAbilityFour(const FInputActionValue& Value);
	void OnAbilityFive(const FInputActionValue& Value);
	void OnAbilitySix(const FInputActionValue& Value);
	void ToggleCameraBool(const FInputActionValue& Value);
	void HoldCamera(const FInputActionValue& Value);
	void ReleaseHoldCamera(const FInputActionValue& InputActionValue);
	void AttackMove(const FInputActionValue& Value);
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	void GiveInitialAbilities();

	UFUNCTION(Server, Reliable)
	void S_CancelAllAbilities();
	
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
	void OnManaChanged(const FOnAttributeChangeData& Data);

	FGameplayEventData BufferAbility;
	bool bShouldActivateBuffer = false;
	void SetFPS();
	
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable)
	void SetRotationLock(bool RotationLocked, FVector TargetDir = FVector::ZeroVector);
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void S_SetRotationLock(bool RotationLocked, FVector TargetDir);

	void ResetDestination();

	UFUNCTION(BlueprintCallable)
	void SetDestination(FVector NewDestination);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void M_SetInfoBarVisibility(bool bVisible);

	void SetOwnHealthBarColor();
	void SetHealthColor(const FLinearColor Color);

	void SetAllHealthBarColors();

	FVector GetHalfHeightVector();

	URTAbilitySystemComponent* GetAbilitySystemComponent() { return AbilitySystemComponent; }
private:
	
	bool HasTag(FString Tag);
	void HandleCamera();

	virtual void Restart() override;

	virtual void PostInitializeComponents() override;

	virtual void PostLoad() override;
	float FPS;
};