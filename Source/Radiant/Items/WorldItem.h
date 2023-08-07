// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
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

	UPROPERTY(EditAnywhere, Replicated)
	uint32 Amount;

	UPROPERTY(ReplicatedUsing=OnRep_Position)
	FVector3d Location;
	
public:
	void InitItem(FName NewItemName, uint32 NewAmount);
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
	UFUNCTION()
	void OnRep_Position();
	float Speed = 1000.0f;
	UPROPERTY()
	class AAvatar* Target;
	class TArray<AAvatar*> PotentialTargets;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
