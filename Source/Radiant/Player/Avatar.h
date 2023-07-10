// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Characters/RTCharacter.h"
#include "GAS/AttributeSets/RTAvatarAttributeSet.h"
#include "Player/RTPlayerState.h"
#include "Util/Interfaces/Carrier.h"
#include "Util/Managers/GridManager.h"
#include "Avatar.generated.h"

UCLASS()
class RADIANT_API AAvatar : public ARTCharacter, public ICarrier
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
	UFUNCTION(Client, Reliable)
	void S_StopMovement();
	void StopMovement();

	UFUNCTION(Client, Reliable)
	void LevelUp(float GetLevel);
	void SetOverheadBarText(const FString& String);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector Destination;
	
	uint8 bAtDestination : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class AActor* Target;
	
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
	TArray<TSubclassOf<class UGameplayEffect>> InitialEffects;
	
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* SystemTemplate;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* AttackMoveSystemTemplate;

	UPROPERTY(EditAnywhere)
	USoundBase* FailedSound;

	UPROPERTY()
	class AGridManager* GridManager;

	/** TESTING **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TESTING)
	TMap<FString, TSubclassOf<class AActor>> DebugSpawnableItems;

protected:
	
	virtual void BeginPlay() override;
	static FVector2D GetMousePosition();
	FHitResult GetMousePositionInWorld() const;
	
	UFUNCTION(Server, Reliable)
	void S_SpawnActorAtMouse(const FString& PieceName, const uint32 Amount, const FVector& Location);

	UFUNCTION(Exec)
	void SpawnActorAtMouse(const FString& PieceName, const uint32 Amount);
	
public:
	UFUNCTION(Server, Reliable)
	void S_PlaceGridPiece(FGridPiece Piece);

	virtual UInventoryComponent* GetInventory() const override;
	virtual FVector GetCarrierLocation() const override { return GetActorLocation(); }
	
	bool CheckShouldAttack();
	void ApplyInitialEffects();
	void CastAbility(const FGameplayTag& AbilityTag);
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
	void MoveCamera(FVector Dir);

	ARTPlayerState* GetRTPlayerState() const { return GetPlayerState<ARTPlayerState>(); }

	virtual void PossessedBy(AController* NewController) override;
	void OnXPChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnLevelChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnRadianiteChanged(const FOnAttributeChangeData& OnAttributeChangeData) const;
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Controller() override;
	void SetHUDIcons(const TMap<EItemSlotID, struct FItemSlotInfo>& AbilityMap);

	virtual ETeamId GetTeamId() const override;
	void GiveInitialAbilities();

	UFUNCTION(Server, Reliable)
	void S_CancelAllAbilities();

	class ARTHUD* GetRTHUD();

	UPROPERTY()
	class URTAvatarAttributeSet* AttributeSetBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	class UWidgetComponent* OverHeadInfoBarWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraMovementThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraMovementSpeed;

	UPROPERTY()
	class UHeroInfoBar* OverHeadInfoBar;

	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnManaChanged(const FOnAttributeChangeData& Data);

	FGameplayEventData BufferAbility;
	bool bShouldActivateBuffer = false;
	void ShowStats();
	
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

	bool HasTag(FString Tag);
	FVector GetHalfHeightVector();

	void BasicAttack();
private:
	
	void HandleCamera(float DeltaSeconds);

	virtual void Restart() override;

	virtual void PostInitializeComponents() override;

	virtual void PostLoad() override;
	float FPS;
};
