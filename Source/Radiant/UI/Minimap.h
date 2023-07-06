// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Minimap.generated.h"


UCLASS()
class RADIANT_API UMinimap : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	class UImage* MinimapImage;
	
	UPROPERTY(EditAnywhere)
	class UMaterialInterface* Material;

	UPROPERTY(EditAnywhere)
	class UTextureRenderTarget2D* RenderTarget;
	
	UPROPERTY(EditAnywhere)
	class UTexture2D* AllyTexture;

	UPROPERTY(EditAnywhere)
	class UTexture2D* EnemyTexture;

	float UnitSize = 13.f;
	float HalfUnitSize = UnitSize * .5f;

public:
	void DrawDynamic();
	void DrawStatic();
};
