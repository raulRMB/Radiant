// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Enums/TeamId.h"
#include "GameFramework/Actor.h"
#include "WorldItem.generated.h"

UCLASS()
class RADIANT_API AWorldItem : public AActor
{
	GENERATED_BODY()	

	UPROPERTY(VisibleAnywhere, Category = Components, meta = (AllowPrivateAccess =true))
	class USphereComponent* PickUpRadius;

	UPROPERTY(VisibleAnywhere, Category = Components, meta = (AllowPrivateAccess =true))
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category = Components, meta = (AllowPrivateAccess =true))
	class UNiagaraComponent* FX;

	UPROPERTY(VisibleAnywhere, Category = Components, meta = (AllowPrivateAccess =true))
	class UWidgetComponent* NameWidget;

	UPROPERTY(EditAnywhere)
	class UDataTable* ItemDataTable;
protected:
	UPROPERTY(EditAnywhere, Replicated)
	FName ItemName;

	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_Amount)
	uint32 Amount;

	UPROPERTY(ReplicatedUsing=OnRep_Location)
	FVector Location;
	uint8 bLocationDirty : 1;
	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;
	bool IsVisibleForTeam(ETeamId TargetTeamId) const;

public:
	UPROPERTY()
	class AGridManager* GridManager;
	void InitItem(FName NewItemName, uint32 NewAmount);
	void UpdateAmount(uint32 NewAmount);
	uint32 GetAmount() const { return Amount; }
	void PickUp(class ICarrier* Carrier);
public:
	UFUNCTION()
	void OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                             const FHitResult& SweepResult);
	AWorldItem();

	FName GetItemName() const { return ItemName; }
protected:
	UFUNCTION()
	void OnEndCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	FTimerHandle SetBackroundTimerHandle;
	void SetBackgroundSize();
	float Speed = 1000.0f;
	void UpdateInfo();
	UFUNCTION()
	void OnRep_Location();
	UPROPERTY()
	class AAvatar* Target;
	class TArray<AAvatar*> PotentialTargets;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION()
	void OnRep_Amount();
};
