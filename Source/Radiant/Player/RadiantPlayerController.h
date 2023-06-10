// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "RadiantPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RADIANT_API ARadiantPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	class ARTPlayerStart* PlayerStart;
public:
	UFUNCTION(Server, Reliable)
	void PlayerLoaded();

	UFUNCTION(Server, Reliable)
	void S_SetPlayerStart(class ARTPlayerStart* NewPlayerStart);

	class ARTPlayerStart* GetPlayerStart() const { return PlayerStart; }
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
}; 
