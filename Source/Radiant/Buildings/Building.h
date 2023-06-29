#pragma once

#include "AbilitySystemInterface.h"
#include "Util/Interfaces/TeamMember.h"
#include "Building.generated.h"

UCLASS()
class ABuilding : public AActor, public IAbilitySystemInterface, public ITeamMember
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Stats, meta=(AllowPrivateAccess=true))
	float MaxHealth = 500.f;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Ability, meta=(AllowPrivateAccess=true))
	TArray<TSubclassOf<class UGameplayAbility>> Abilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Team, meta=(AllowPrivateAccess=true))
	ETeamId TeamId = ETeamId::Neutral;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, meta=(AllowPrivateAccess=true))
	class URTAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, meta=(AllowPrivateAccess=true))
	class UTowerAttributeSet* AttributeSet;
	void BeginPlay();
	void GiveInitialAbilities();
public:	
	// Sets default values for this actor's properties
	ABuilding();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UTowerAttributeSet* GetAttributeSet() const;
	virtual ETeamId GetTeamId() const override { return TeamId; }
};
