// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Combat/Artillery/LinearSkillshot.h"
#include "Hook.generated.h"


UCLASS()
class RADIANT_API AHook : public ALinearSkillshot
{
	GENERATED_BODY()

	UPROPERTY()
	class UGAHook* HookAbility;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = true))
	class UCableComponent* CableComponent;

	UPROPERTY(Replicated)
	class AActor* Hooker;
public:	
	AHook();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void AttachToHooker(AActor* Hooker);
	
	void SetAbility(UGAHook* NewJookAbility) { HookAbility = NewJookAbility; }
	
	virtual void OverlapStart(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	 
	virtual void LifeSpanExpired() override;
	virtual void Tick(float DeltaTime) override;
};
