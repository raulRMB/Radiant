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
	//UPROPERTY(Replicated, VisibleAnywhere)
	UPROPERTY(VisibleAnywhere)
	TArray<EEnvironmentType> Cells;
	UPROPERTY(Replicated, VisibleAnywhere)
	TArray<bool> VisibleCells;
	
	UPROPERTY(EditAnywhere)
	uint16 GridDimensions;
	
	UPROPERTY(EditAnywhere)
	uint8 bSpawnMap : 1;

	UPROPERTY(EditAnywhere)
	TMap<EEnvironmentType, TSubclassOf<class AActor>> BuildingTypes;

	float PerlinScale = 0.1f;

	UPROPERTY(EditAnywhere)
	class UTexture2D* MapTexture;

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> VisibleActors;
	
public:
	AGridManager();

	UFUNCTION(CallInEditor)
	void GenerateMap();
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
	
private:
	UFUNCTION(NetMulticast, Reliable)
	void M_UpdateGrid(const FIntVector2& Position, EEnvironmentType EnvironmentType);	

	void CheckVisible(const FIntVector2& Start);

	bool GetVisibleCell(const FIntVector2& Position);

	void ClearVision();
	void SetVisible();
	
	virtual void Tick(float DeltaSeconds) override;

	//void UpdateTexture(const FIntVector2& Pos);

	UPROPERTY(EditAnywhere)
	class UTexture2D* EmptyTexture;
	UPROPERTY(EditAnywhere)
	class UTexture2D* VisionTexture;
	UPROPERTY(EditAnywhere)
	class UTexture2D* PlayerTexture;

	UPROPERTY(EditAnywhere, Category = "Vision")
	float VisionRange = 10.f;
};
