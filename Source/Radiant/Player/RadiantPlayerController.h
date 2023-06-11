// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "RadiantPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API ARadiantPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = true))
	class ARTPlayerStart* PlayerStart;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = true))
	class UInputAction* ClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = true))
	class UInputAction* AttackMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = true))
	class UInputAction* CameraToggleAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = true))
	class UInputAction* CameraHoldAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = true))
	class UInputAction* AbilityOneAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = true))
	class UInputAction* AbilityTwoAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = true))
	class UInputAction* AbilityThreeAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = true))
	class UInputAction* AbilityFourAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = true))
	class UInputAction* AbilityFiveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = true))
	class UInputAction* AbilitySixAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = true))
	class UInputMappingContext* MappingContext;
public:
	UFUNCTION(Server, Reliable)
	void PlayerLoaded();

	UFUNCTION(Server, Reliable)
	void S_SetPlayerStart(class ARTPlayerStart* NewPlayerStart);

	class ARTPlayerStart* GetPlayerStart() const { return PlayerStart; }
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;

	UFUNCTION()
	void OnClick(const FInputActionValue& Value);
	
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
}; 
