// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Characters/RTCharacter.h"
#include "GAS/AttributeSets/RTAvatarAttributeSet.h"
#include "Util/Interfaces/Carrier.h"
#include "Util/Managers/Grid/GridManager.h"
#include "Avatar.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FGridUpdateSignature, const FIntVector2&, EEnvironmentType);

UCLASS()
class RADIANT_API AAvatar : public ARTCharacter, public ICarrier
{
	GENERATED_BODY()

public:
	AAvatar();
	void SetMeshForClass(EClassType Class);

	UPROPERTY(EditAnywhere, Category=CharacterSwapping)
	TMap<EClassType, USkeletalMesh*> AvatarMeshes;

	UPROPERTY(EditAnywhere, Category=CharacterSwapping)
	TMap<EClassType, TSubclassOf<UAnimInstance>> AvatarAnimBlueprints;

	UPROPERTY(EditAnywhere, Category=CharacterSwapping)
	TMap<EClassType, class UAnimMontage*> DeathAnimMontages;

	UFUNCTION(Server, Reliable)
	void S_ToggleItemMagnet();
	UFUNCTION(NetMulticast, Reliable)
	void M_SwitchMesh(EClassType Class);
	bool IsMagnetized;
	void GameReady();
	void OnAbilityFailed(const UGameplayAbility* GameplayAbility, const FGameplayTagContainer& GameplayTags);
	void CastingTagChanged(FGameplayTag GameplayTag, int I);
	
	UFUNCTION(Client, Reliable)
	void S_StopMovement();
	void StopMovement();

	UFUNCTION(Client, Reliable)
	void LevelUp(float GetLevel);
	void SetOverheadBarText(const FString& String);
	bool TryPickupItem();

	UFUNCTION(Exec)
	void GiveItem(const FName& ItemName, int32 Amount);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector Destination;

	uint8 bAtDestination : 1;
	uint8 bIsDragging : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class AActor* Target;

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

	UPROPERTY(EditAnywhere)
	class UAnimMontage* DeathMontage;
protected:
	void SetIsDraggingFalse();
	void OnDragStatusChanged(bool bArg);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	static FVector2D GetMousePosition();
	FHitResult GetMousePositionInWorld(bool bIgnoreSelf = false) const;

	UFUNCTION(Server, Reliable)
	void S_SpawnActorAtMouse(const FString& PieceName, const uint32 Amount, const FVector& Location);

	UFUNCTION(Exec)
	void SpawnActorAtMouse(const FString& PieceName, const uint32 Amount);
	UFUNCTION(Exec)
	void SpawnActorAtSelf(const FString& PieceName, const uint32 Amount);
	UFUNCTION(Exec)
	void GiveArchetypeItems(const EClassType ClassType);
	UFUNCTION(Exec)
	void ClearInventory();

public:
	UFUNCTION(Server, Reliable)
	void S_TryPlaceGridPiece(FGridPiece Piece, FName ItemName);
	UFUNCTION(Server, Reliable)
	void S_PickUpItem(class AWorldItem* WorldItem);

	virtual UInventoryComponent* GetInventory() const override;
	virtual FVector GetCarrierLocation() const override { return GetActorLocation(); }

	bool CheckShouldAttack();
	void CastHotBarAbility(const FGameplayTag& AbilityTag, bool bIgnoreSelf = false);
	void ApplyInitialEffects();
	void CastAbility(const FGameplayTag& AbilityTag, bool bIgnoreSelf = false);
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
	void Demolish();
	UFUNCTION(Server, Reliable)
	void S_Demolish(ETeamId PlayerTeamId, ABuilding* Building);

	class ARTPlayerState* GetRTPS() const;
	
	virtual void PossessedBy(AController* NewController) override;
	void OnXPChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnLevelChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnMaxHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Controller() override;

	virtual ETeamId GetTeamId() const override;
	void GiveInitialAbilities();

	virtual void SetIsDead(const bool NewIsDead) override;

	UFUNCTION(NetMulticast, Reliable)
	void M_PlayDeathMontage();

	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;
	bool IsVisibleForTeam(ETeamId TargetTeamId) const;

	UFUNCTION(Server, Reliable)
	void S_CancelAllAbilities();

	class ARTHUD* GetRTHUD();

	UPROPERTY()
	class URTAvatarAttributeSet* AttributeSet;

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
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void C_SetRotationLock(bool RotationLocked, FVector TargetDir = FVector::ZeroVector);
	
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
