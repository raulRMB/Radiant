// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Enums/EnvironmentType.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"


class ATeamGridManager;
struct FGridPiece;

DECLARE_MULTICAST_DELEGATE_TwoParams(FGridUpdateSignature, const FIntVector2&, EEnvironmentType);
DECLARE_MULTICAST_DELEGATE_OneParam(FGridInitSignature, const TArray<EEnvironmentType>&);

UCLASS()
class RADIANT_API AGridManager : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<EEnvironmentType> Cells;
	
	UPROPERTY(EditAnywhere)
	uint16 GridDimensions;
	
	UPROPERTY(EditAnywhere)
	uint8 bSpawnMap : 1;

	UPROPERTY(EditAnywhere)
	TMap<EEnvironmentType, TSubclassOf<class AActor>> BuildingTypes;

	UPROPERTY(EditAnywhere)
	class UTexture2D* MapTexture;

	UPROPERTY(VisibleAnywhere)
	TArray<ATeamGridManager*> TeamGridManagers;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ATeamGridManager> TeamGridManagerClass;
	
public:
	AGridManager();

	UFUNCTION(CallInEditor)
	void GenerateMap();

	void InitGrid();

	void AddVisibleActor(AActor* Actor);

	uint32 CellSize = 200;
	uint32 CellHalfSize = CellSize / 2;

	void PlacePieceAtMouse(const FGridPiece Piece);
	void ClearPiece(const FGridPiece Piece);
	bool CheckCanPlace(const FGridPiece Piece);

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class UTexture2D* RenderTarget;
	
public:
	FGridUpdateSignature OnGridUpdate;

private:
	EEnvironmentType& GetCell(const FIntVector2& Position);

	FVector GetTransformedVector(const FIntVector2& Position);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaSeconds) override;
};
