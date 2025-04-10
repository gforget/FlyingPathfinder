// Copyright(c) 2025 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Utility/GenericStack.h"
#include "GameFramework/Volume.h"
#include "FlyingPathfindingNode.h"
#include "FlyingPathfinderVolume.generated.h"

UCLASS()
class FLYINGPATHFINDER_API AFlyingPathfinderVolume : public AVolume
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlyingPathfinderVolume();

	GenericStack<UFlyingPathfindingNode*> GetPathToDestination(FVector InitialPoint, FVector DestinationPoint);
	
protected:
	virtual void BeginPlay() override;

	// Called when actor is moved, rotated or scaled in the editor
	virtual void OnConstruction(const FTransform& Transform) override;
	
#if WITH_EDITOR
	// Called when a property is changed in the editor
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	// Called when actor is moved in the editor
	virtual void PostEditMove(bool bFinished) override;
	
	void UpdateDebugVisualization();
	void GeneratePathfindingNodes();
	void ClearPathfindingNodes();

#endif
	
	// Array of created pathfinding nodes
	UPROPERTY()
	TArray<UFlyingPathfindingNode*> PathfindingNodes;

	// 3D grid to store references to nodes by position - For optimized search of the Nodes
	UPROPERTY()
	TMap<FIntVector, UFlyingPathfindingNode*> NodeGrid;
	
	// Array to store temporary nodes
	UPROPERTY()
	TArray<UFlyingPathfindingNode*> TemporaryNodes;
	
	UPROPERTY(EditAnywhere, Category = "Volume Properties", meta = (ClampMin = "50.0"))
	FVector GridSpacing = FVector(200.0f, 200.0f, 200.0f);
	
	// Collision detection for nodes
	UPROPERTY(EditAnywhere, Category = "Volume Properties", meta = (ClampMin = "10.0", UIMin = "10.0"))
	float BlockCheckRadius = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Volume Properties", meta = (ClampMin = "10.0", UIMin = "10.0"))
	float NeighbourConnectionTraceRadius = 50.0f;
	
	//Debug properties
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShowDebugSphere = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	int DebugIndexNode = 0;
	
	UPROPERTY(EditAnywhere, Category = "Debug", meta = (EditCondition = "bShowDebugSphere"))
	float DebugSphereRadius = 20.0f;
	
	UPROPERTY(EditAnywhere, Category = "Debug", meta = (EditCondition = "bShowDebugSphere"))
	FColor DebugSphereColor = FColor::Green;

public:
	virtual void Tick(float DeltaTime) override;

private:
	// Find the closest pathfinding node to a given world position
	UFlyingPathfindingNode* FindClosestNode(const FVector& Point);
	
	// Create a temporary node at the given location and connect it to nearby nodes
	UFlyingPathfindingNode* CreateTemporaryNode(const FVector& Location);
	
	// Clear all temporary nodes and their connections
	void ClearTemporaryNodes();
	
	// Check if a position is blocked by solid geometry
	bool IsPositionBlocked(const FVector& Position, float Radius);

	// Return all nodes from a radius set in Grid Metric (not world metrics)
	TArray<UFlyingPathfindingNode*> FindNodesInRadius(const FVector& Point, int32 Radius);

	
	bool HasClearSphereLineOfSight(const FVector& Start, const FVector& End);
};
