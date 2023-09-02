// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Minimap.generated.h"


enum class EEnvironmentType : uint8;

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
	class UTextureRenderTarget2D* MapRenderTarget;
	
	UPROPERTY(EditAnywhere)
	class UTexture2D* AllyTexture;

	UPROPERTY(EditAnywhere)
	class UTexture2D* EnemyTexture;

	float UnitSize = 13.f;
	float HalfUnitSize = UnitSize * .5f;

	UPROPERTY(EditAnywhere)
	TMap<EEnvironmentType, class UTexture2D*> EnvironmentTextures;
	
public:
	void DrawDynamic();
	void BindGridUpdate();
	void InitGrid(const TArray<EEnvironmentType>& Array);

private:
	UFUNCTION()
	void OnGridUpdate(const FIntVector2& Position, EEnvironmentType EnvironmentType);
	UFUNCTION()
	void Request();
};
