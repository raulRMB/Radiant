// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Enums/EnvironmentType.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"


class ATeamGridManager;
struct FGridPiece;
enum class EVisionRange : uint8;

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

	UPROPERTY(EditAnywhere)
	class UTexture2D* VisionRangesTexture;

	UPROPERTY(VisibleAnywhere)
	TArray<FIntVector2> ShortRange;
	UPROPERTY(VisibleAnywhere)
	TArray<FIntVector2> MediumRange;
	UPROPERTY(VisibleAnywhere)
	TArray<FIntVector2> LongRange;
	
public:
	AGridManager();

	UFUNCTION(CallInEditor)
	void GenerateMap();

	UFUNCTION(CallInEditor)
	void GenerateVisionRanges();

	TArray<FIntVector2>& GetRange(EVisionRange Range);
	
	void InitGrid();

	void AddVisibleActor(AActor* Actor);
	void PieceChanged(FGridPiece Piece);

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
	UStaticMeshComponent* GetMesh(EEnvironmentType Type);
	EEnvironmentType& GetCell(const FIntVector2& Position);
	class ABuilding* GetBuildingType(EEnvironmentType Type);
private:

	FVector GetTransformedVector(const FIntVector2& Position);
	FIntVector2 GetTransformedPosition(const FIntVector2& Position);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaSeconds) override;
};
