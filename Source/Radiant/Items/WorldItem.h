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
	
public:
	void InitItem(FName NewItemName, uint32 NewAmount);
	void PickUp(class ICarrier* Carrier);
public:
	UFUNCTION()
	void MoveToActorIfMagnetized(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                             const FHitResult& SweepResult);
	AWorldItem();

	FName GetItemName() const { return ItemName; }
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	FTimerHandle SetBackroundTimerHandle;
	void SetBackgroundSize();
	class AAvatar* Target;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
