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
	FName ItemName;
public:
	void InitItem(FName Name);
	
	void PickUp(class ICarrier* Carrier);
public:	
	AWorldItem();

	FName GetItemName() const { return ItemName; }
protected:
	virtual void BeginPlay() override;
};
