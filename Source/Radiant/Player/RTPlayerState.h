// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "RTPlayerController.h"
#include "Data/ItemData.h"
#include "GameFramework/PlayerState.h"
#include "UI/ItemSlot.h"
#include "Util/Interfaces/Carrier.h"
#include "Util/Interfaces/Killable.h"
#include "Util/Interfaces/TeamMember.h"
#include "RTPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FUpdateRadianiteSignature, float);

class AAvatar;

UCLASS()
class RADIANT_API ARTPlayerState : public APlayerState, public IAbilitySystemInterface, public ITeamMember, 
								   public ICarrier, public IKillable
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

	UPROPERTY(Replicated, VisibleAnywhere)
	uint8 bIsDead : 1;
public:
	virtual FVector GetCarrierLocation() const override;
	virtual class UInventoryComponent* GetInventory() const override;
	void RemoveAbility(FGameplayAbilitySpecHandle Handle);
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
	FGameplayAbilitySpecHandle GiveAbility(FItemData* ItemData);
	TArray<class UAbilityDataAsset*> GetInnateAbilities() const;

#pragma region Killable
	
	UFUNCTION(Server, Reliable)
	void S_SetIsDead(bool bNewIsDead);
	virtual void SetIsDead(const bool bIsDead) override;
	virtual bool GetIsDead() const override { return bIsDead; }
	void StartRespawnTimer();
	UFUNCTION()
	void Respawn();

#pragma endregion Killable
	
#pragma region ConvenienceGetters

	class ARTPlayerController* GetRTPC() const;
	class AAvatar* GetAvatar() const;
	class ARTGameMode* GetRTGM() const;
	class ARTGameState* GetRTGS() const;
	
#pragma endregion ConvenienceGetters

};
