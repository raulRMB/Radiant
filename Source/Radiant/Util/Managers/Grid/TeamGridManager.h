// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Building/Building.h"
#include "Characters/RTCharacter.h"
#include "GameFramework/Actor.h"
#include "Util/Interfaces/TeamMember.h"
#include "TeamGridManager.generated.h"

class ANotificationActor;
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

	UPROPERTY()
	TArray<ANotificationActor*> NotificationActors;
	
	UFUNCTION()
	void OnRep_Cells();
	void SpawnInitialTempActors();

	bool Initialized = false;
	bool TempActorsEnabled = true;
	UPROPERTY()
	TArray<class AActor*> TempPieces;
	
	UPROPERTY(EditAnywhere)
	uint16 GridDimensions;

	UPROPERTY(Replicated, VisibleAnywhere)
	TArray<AActor*> VisibleActors;

	uint32 CellSize = 200;
	uint32 CellHalfSize = CellSize / 2;

	UPROPERTY(EditAnywhere)
	class UTexture2D* FOWRenderTarget;

	UPROPERTY(EditAnywhere)
	class UTexture2D* MinimapRenderTarget;
	
	UPROPERTY(Replicated, EditAnywhere)
	ETeamId TeamId = ETeamId::Neutral;

	UPROPERTY(Replicated)
	class AGridManager* GridManager;

	UPROPERTY(EditAnywhere)
	TMap<class AActor*, int32> ActorsInVision;
	
public:
	ATeamGridManager();

	const TMap<AActor*, int32>& GetActorsInVision();
	void AddActorToActorsInVision(class AActor* Actor);
	void DecrementActorFromActorsInVision(class AActor* Actor);
	void RemoveActorFromActorsInVision(class AActor* Actor);
	bool IsTargetInVision(const AActor* ArtCharacter) const;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;

public:
	virtual void Tick(float DeltaTime) override;
	void DrawMiniMap();

	void InitGrid();

	void AddVisibleActor(AActor* Actor);
	void RemoveVisibleActor(AActor* Actor);
	FVector GetTransformedVector(const FIntVector2& Position);

	void Init(const TArray<EEnvironmentType> TrueCells, TMap<EEnvironmentType, TSubclassOf<AActor>> BuildingTypes);

	virtual ETeamId GetTeamId() const override { return TeamId; }
	void SetTeamId(const ETeamId NewTeamId) { TeamId = NewTeamId; }

	bool IsTargetVisible(const AActor* Target);

	void SetGridManager(class AGridManager* NewGridManager) { GridManager = NewGridManager; }
;
	void UpdateTempActor(const FGridPiece& Piece);
	bool HasTempActor(struct FGridPiece& Piece);
	void SpawnTempActor(const FGridPiece& Piece, ABuilding* Building, FTransform Transform);
	void HideTempActor(FGridPiece& Piece, bool bHide);
	void DestroyTempActor(const FGridPiece& Piece);
	UFUNCTION(NetMulticast, Reliable)
	void M_UpdateTempActor(const FGridPiece& Piece);

	void PieceChanged(const ABuilding* Building);
	void UpdateCellForClients(FGridPiece Piece);
	void SpawnNotificationActor(FGridPiece Piece);

private:	
	bool IsBlockingVision(const FIntVector2& Position);
	EEnvironmentType& GetCell(const FIntVector2& Position);
	bool GetNeighbor(const FIntVector2& Position, const FIntVector2& Direction, EEnvironmentType& OutCell);
	bool GetVisibleCell(const FIntVector2& Position);
	void ClearAllVisible();
	void DrawVisible();
	bool CheckVisible(const FVector2D& From, const FVector2D& To);
	FColor GetColorForType(const EEnvironmentType& Type);
};
