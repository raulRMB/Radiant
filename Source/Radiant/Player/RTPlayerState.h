// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/PlayerState.h"
#include "GameplayTags/Classes/GameplayTagContainer.h"
#include "UI/UISlot.h"
#include "Util/Interfaces/Carrier.h"
#include "Util/Interfaces/TeamMember.h"
#include "..\Util\Enums\UISlotID.h"
#include "RTPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FUpdateRadianiteSignature, float);

UCLASS()
class RADIANT_API ARTPlayerState : public APlayerState, public IAbilitySystemInterface, public ITeamMember, public ICarrier
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	class URTAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class URTAvatarAttributeSet* AttributeSet;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated, VisibleAnywhere)
	class AActor* Target;
	UPROPERTY(Replicated, VisibleAnywhere)
	ETeamId TeamId = ETeamId::Neutral;
	UPROPERTY(ReplicatedUsing=OnRep_UsernameChanged, VisibleAnywhere)
	FString Username = "";

	UPROPERTY(EditAnywhere)
	class UDataTable* ItemDataTable;

	
public:
	virtual FVector GetCarrierLocation() const override;
	virtual class UInventoryComponent* GetInventory() const override;
	class ARTPlayerController* GetRTController() const;
	
	virtual void BeginPlay() override;
	
	FUpdateRadianiteSignature OnUpdateRadianite;
	void OnRadianiteChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	ARTPlayerState();

	UFUNCTION(BlueprintCallable)
	void SetPlayerStats();

	UFUNCTION(Server, Reliable)
	void S_SetTarget(AActor* NewTargetId);

	UFUNCTION()
	virtual void OnRep_UsernameChanged();
	
	AActor* GetTarget() const { return Target; }
	virtual ETeamId GetTeamId() const override { return TeamId; }

	void SetTeamId(ETeamId NewTeamId) { TeamId = NewTeamId; }
	
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	class URTAvatarAttributeSet* GetAttributeSetBase() const;

	UFUNCTION(Server, Reliable)
	void SetUsername(const FString& String);
	FString GetUsername();
	UFUNCTION(Server, Reliable)
	void S_BuyAbility(const FName& AbilityName);
	
	UPROPERTY(Replicated, EditAnywhere)
	TArray<class UAbilityDataAsset*> InnateAbilities;
	TArray<class UAbilityDataAsset*> GetInnateAbilities() const;
};