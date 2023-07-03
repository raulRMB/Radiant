#pragma once

#include "AbilitySystemInterface.h"
#include "Util/Interfaces/Targetable.h"
#include "Util/Interfaces/TeamMember.h"
#include "Building.generated.h"

struct FOnAttributeChangeData;

UCLASS()
class ABuilding : public AActor, public IAbilitySystemInterface, public ITeamMember, public ITargetable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Stats, meta=(AllowPrivateAccess=true))
	float MaxHealth = 500.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Components, meta=(AllowPrivateAccess=true))
	class UCapsuleComponent* CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Components, meta=(AllowPrivateAccess=true))
	class UWidgetComponent* InfoBarWidgetComponent;
	
protected:
	UPROPERTY()
	class UAIInfoBar* InfoBar;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Ability, meta=(AllowPrivateAccess=true))
	TArray<TSubclassOf<class UGameplayAbility>> Abilities;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category=Team, meta=(AllowPrivateAccess=true))
	ETeamId TeamId = ETeamId::Neutral;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, meta=(AllowPrivateAccess=true))
	class URTAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, meta=(AllowPrivateAccess=true))
	class UBuildingAttributeSet* AttributeSet;
	
	virtual void BeginPlay() override;
	void GiveInitialAbilities();
	
	virtual void Tick(float DeltaSeconds) override;

public:
	ABuilding();

	void SetHealthBarColor();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UBuildingAttributeSet* GetAttributeSet() const;
	virtual ETeamId GetTeamId() const override { return TeamId; }

	UFUNCTION(Server, Reliable)
	void S_SetTeamId(ETeamId NewTeamId);

private:
	void OnHealthChanged(const FOnAttributeChangeData& Data);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
