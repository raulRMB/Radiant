#pragma once

#include "AbilitySystemInterface.h"
#include "Util/Interfaces/Targetable.h"
#include "Util/Interfaces/TeamMember.h"
#include "Structs/GridPiece.h"
#include "Util/Managers/Grid/GridManager.h"
#include "Util/Util.h"
#include "Building.generated.h"

struct FOnAttributeChangeData;


UCLASS()
class ABuilding : public AActor, public IAbilitySystemInterface, public ITeamMember, public ITargetable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Replicated)
	AActor* TmpActor;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Stats, meta=(AllowPrivateAccess=true))
	float MaxHealth = 200.f;  
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Components, meta=(AllowPrivateAccess=true))
	class UBoxComponent* Box;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Components, meta=(AllowPrivateAccess=true))
	class UWidgetComponent* InfoBarWidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, meta=(AllowPrivateAccess=true))
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, meta=(AllowPrivateAccess=true))
	class USceneComponent* Root;
	
	UPROPERTY(EditAnywhere, meta=(MakeEditWidget))
	FVector WidgetLocation;

	UPROPERTY(Replicated)
	FGridPiece GridPiece;
	
	UPROPERTY()
	class AGridManager* GridManager;

	UPROPERTY(EditAnywhere, Replicated)
	uint8 bHideLevel : 1;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWorldItem> DropItemClass;

	UPROPERTY(EditAnywhere)
	FName DropItemName;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> InfoBarWidgetClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UActorComponent> InfoBarActorComponent;

	UPROPERTY(EditAnywhere)
	class UMaterialInterface* DarkMaterial;

	UPROPERTY()
	class UActorManager* ActorManager;

protected:
	UPROPERTY()
	class UAIInfoBar* InfoBar;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Ability, meta=(AllowPrivateAccess=true))
	TArray<TSubclassOf<class UGameplayAbility>> Abilities;

	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;
	bool IsVisibleForTeam(ETeamId TargetTeamId) const;

	bool bShouldShow = true;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category=Team, meta=(AllowPrivateAccess=true))
	ETeamId TeamId = ETeamId::Neutral;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, meta=(AllowPrivateAccess=true))
	class URTAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere)
	class URTBaseAttributeSet* AttributeSet;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AActor> DestroyParticles;

	virtual void BeginPlay() override;
	void GiveInitialAbilities();

	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	ABuilding();
	UStaticMeshComponent* GetMesh() const;

	const FGridPiece& GetGridPiece() const { return GridPiece; }
	void SetHealthBarColor();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	URTBaseAttributeSet* GetAttributeSet() const;
	virtual ETeamId GetTeamId() const override { return TeamId; }

	UMaterialInterface* GetDarkMaterial() const { return DarkMaterial; }

	UFUNCTION(Server, Reliable)
	void S_SetTeamId(ETeamId NewTeamId);
	UFUNCTION(Server, Reliable)
	void S_Demolish();
	UFUNCTION(NetMulticast, Reliable)
	void M_ShowInfoBar(const bool bCond, const float HealthPercent);

	void InitGridValues(class AGridManager* NewGridManager, const FGridPiece& NewGridPiece);
	
	void DestroyBuilding();
	const UBoxComponent* GetBox() const { return Box; }

	FDeathNotifySignature OnDeathNotify;
	
private:
	
	void OnHealthChanged(const FOnAttributeChangeData& Data);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
