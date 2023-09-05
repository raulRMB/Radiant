// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Util/Interfaces/TeamMember.h"
#include "TeamGridManager.generated.h"

enum class ETeamId : uint8;
enum class EEnvironmentType : uint8;

UCLASS()
class RADIANT_API ATeamGridManager : public AActor, public ITeamMember
{
	GENERATED_BODY()	

	UPROPERTY(ReplicatedUsing=OnRep_Cells, VisibleAnywhere)
	TArray<EEnvironmentType> Cells;

	UPROPERTY()
	TArray<bool> VisibleCells;

	UFUNCTION()
	void OnRep_Cells();
	void SpawnInitialTempActors();

	bool Initialized = false;
	
	UPROPERTY()
	TArray<class AActor*> TempPieces;
	
	UPROPERTY(EditAnywhere)
	uint16 GridDimensions;

	UPROPERTY(Replicated, VisibleAnywhere)
	TArray<AActor*> VisibleActors;

	uint32 CellSize = 200;
	uint32 CellHalfSize = CellSize / 2;

	UPROPERTY(EditAnywhere)
	class UTexture2D* RenderTarget;

	UPROPERTY(Replicated, EditAnywhere)
	ETeamId TeamId = ETeamId::Neutral;

	UPROPERTY(Replicated)
	class AGridManager* GridManager;
	
public:
	ATeamGridManager();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;

public:
	virtual void Tick(float DeltaTime) override;

	void InitGrid();

	void AddVisibleActor(AActor* Actor);
	void RemoveVisibleActor(AActor* Actor);
	bool HasTempActor(struct FGridPiece& Piece);
	void SpawnTempActor(const FGridPiece& Piece, UStaticMeshComponent* Mesh, FTransform Transform);
	void HideTempActor(FGridPiece& Piece, bool bHide);
	void DestroyTempActor(const FGridPiece& Piece);
	FVector GetTransformedVector(const FIntVector2& Position);

	void Init(const TArray<EEnvironmentType> TrueCells, TMap<EEnvironmentType, TSubclassOf<AActor>> BuildingTypes);

	virtual ETeamId GetTeamId() const override { return TeamId; }
	void SetTeamId(const ETeamId NewTeamId) { TeamId = NewTeamId; }

	bool IsTargetVisible(const AActor* Target);

	void SetGridManager(class AGridManager* NewGridManager) { GridManager = NewGridManager; }
	void UpdateTempActor(const FGridPiece& Piece);

	UFUNCTION(NetMulticast, Reliable)
	void M_UpdateTempActor(const FGridPiece& Piece);
	
	void PieceChanged(const FGridPiece& Piece);

private:	
	bool IsBlockingVision(const FIntVector2& Position, const FIntVector2& Direction);
	EEnvironmentType& GetCell(const FIntVector2& Position);
	bool GetNeighbor(const FIntVector2& Position, const FIntVector2& Direction, EEnvironmentType& OutCell);
	bool GetVisibleCell(const FIntVector2& Position);
	void ClearAllVisible();
	void DrawVisible();
	bool CheckVisible(const FVector2D& From, const FVector2D& To);
	void UpdateGridIfOutOfSync(FIntVector2 Pos);
};
