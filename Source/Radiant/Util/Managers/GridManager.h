// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Enums/TeamId.h"
#include "Enums/EnvironmentType.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

USTRUCT()
struct FGridPiece
{
	GENERATED_BODY()

	UPROPERTY()
	EEnvironmentType Type;
	UPROPERTY()
	uint32 Level;
	UPROPERTY()
	FIntVector2 Position;
	UPROPERTY()
	uint8 Size;
	UPROPERTY()
	ETeamId TeamId;

	FGridPiece() :
		Type(EEnvironmentType::EEnvironmentType_Empty),
		Level(0),
		Position(FIntVector2(0, 0)),
		Size(1),
		TeamId(ETeamId::Neutral)
	{}
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FGridUpdateSignature, const FIntVector2&, EEnvironmentType);
DECLARE_MULTICAST_DELEGATE_OneParam(FGridInitSignature, const TArray<EEnvironmentType>&);

UCLASS()
class RADIANT_API AGridManager : public AActor
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<class AActor*> Pieces;
	UPROPERTY()
	TArray<class AActor*> TmpPieces;
	//UPROPERTY(Replicated, VisibleAnywhere)
	UPROPERTY(VisibleAnywhere)
	TArray<EEnvironmentType> Cells;
	UPROPERTY(VisibleAnywhere)
	TArray<EEnvironmentType> TmpCells;
	//UPROPERTY(Replicated, VisibleAnywhere)
	TArray<bool> VisibleCellsRedTeam;
	TArray<bool> VisibleCellsBlueTeam;

	TArray<bool>& GetVisibleCellsArray(ETeamId TeamId);
	
	UPROPERTY(EditAnywhere)
	uint16 GridDimensions;
	
	UPROPERTY(EditAnywhere)
	uint8 bSpawnMap : 1;

	UPROPERTY(EditAnywhere)
	TMap<EEnvironmentType, TSubclassOf<class AActor>> BuildingTypes;

	// UPROPERTY(EditAnywhere)
	// TMap<EEnvironmentType, TSubclassOf<class AActor>> PlaceholderBuildings;
	
	float PerlinScale = 0.1f;

	UPROPERTY(EditAnywhere)
	class UTexture2D* MapTexture;

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> VisibleActors;
	
public:
	AGridManager();

	UFUNCTION(CallInEditor)
	void GenerateMap();
	void SpawnTmpActor(FGridPiece& Piece, UStaticMeshComponent* Mesh, FTransform Transform);
	void DestroyTmpActor(FGridPiece& Piece);
	void InitGrid();
	UFUNCTION(Server, Reliable)
	void S_RequestInitGrid(class ARTPlayerState* PlayerState);

	void AddVisibleActor(AActor* Actor);
	
	uint32 CellSize = 200;
	uint32 CellHalfSize = CellSize / 2;

	void PlacePieceAtMouse(const FGridPiece Piece);
	void ClearPiece(const FGridPiece Piece);
	bool CheckCanPlace(const FGridPiece Piece);

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere)
	class UTexture2D* RenderTarget;
	
public:
	FGridUpdateSignature OnGridUpdate;
	bool IsTargetVisibleForTeam(const AActor* Target, ETeamId TeamId);

private:
	//UFUNCTION(NetMulticast, Reliable)
	//void M_UpdateGrid(const FIntVector2& Position, EEnvironmentType EnvironmentType);

	bool GetVisibleCell(const TArray<bool>& Arr, const FIntVector2& Position);
	
	void ClearAllVisible(TArray<bool>& Arr);
	void DrawVisible(ETeamId TeamId, TArray<bool>& VisibleCells);
	bool CheckVisible(const FVector2D& From, const FVector2D& To, double Angle, TArray<bool>& VisibleCells);

	bool IsBlockingVision(const FIntVector2& Position, const FIntVector2& Direction);
	EEnvironmentType& GetCell(const FIntVector2& Position);
	bool GetNeighbor(const FIntVector2& Position, const FIntVector2& Direction, EEnvironmentType& OutCell);

	FVector GetTransformedVector(const FIntVector2& Position);

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
	class UTexture2D* EmptyTexture;
	UPROPERTY(EditAnywhere)
	class UTexture2D* VisionTexture;
	UPROPERTY(EditAnywhere)
	class UTexture2D* PlayerTexture;

	UPROPERTY(EditAnywhere, Category = "Vision")
	float VisionRange = 10.f;
};
