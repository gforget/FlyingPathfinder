// Fill out your copyright notice in the Description page of Project Settings.

#include "Pathfinder/FlyingPathfinderVolume.h"
#include "DrawDebugHelpers.h"
#include "Components/BrushComponent.h"

// Sets default values
AFlyingPathfinderVolume::AFlyingPathfinderVolume()
{
	PrimaryActorTick.bCanEverTick = false;
}

GenericStack<UFlyingPathfindingNode*> AFlyingPathfinderVolume::GetPathToDestination(UFlyingPathfindingNode* InitialNode,
	UFlyingPathfindingNode* DestinationNode)
{
	//TODO: get the closest node from the initial position and the closest point from the end destination
	GenericStack<UFlyingPathfindingNode*> PathStack;
	return PathStack;
}

// Called when the game starts or when spawned
void AFlyingPathfinderVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFlyingPathfinderVolume::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	GeneratePathfindingNodes();
	UpdateDebugVisualization();
}

#if WITH_EDITOR
void AFlyingPathfinderVolume::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	GeneratePathfindingNodes();
	UpdateDebugVisualization();
}

void AFlyingPathfinderVolume::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	
	GeneratePathfindingNodes();
	UpdateDebugVisualization();
}

void AFlyingPathfinderVolume::ClearPathfindingNodes()
{
	// Remove all existing nodes
	for (UFlyingPathfindingNode* Node : PathfindingNodes)
	{
		if (Node)
		{
			Node->DestroyComponent();
		}
	}
	
	PathfindingNodes.Empty();
	NodeGrid.Empty();
}

void AFlyingPathfinderVolume::GeneratePathfindingNodes()
{
	// Clear existing nodes first
	ClearPathfindingNodes();
	
	// Only generate nodes in editor
	if (!GetWorld() || !GetWorld()->IsEditorWorld())
	{
		return;
	}
	
	UBrushComponent* BrushComponentPtr = GetBrushComponent();
	if (!BrushComponentPtr)
	{
		return;
	}
	
	// Get the bounds of the volume
	FBox Bounds = BrushComponentPtr->Bounds.GetBox();
	
	// Calculate positions in the grid
	FVector Min = Bounds.Min;
	FVector Max = Bounds.Max;
	
	// Calculate the number of nodes in each dimension
	int32 NumPointsX = FMath::FloorToInt((Max.X - Min.X) / GridSpacing) + 1;
	int32 NumPointsY = FMath::FloorToInt((Max.Y - Min.Y) / GridSpacing) + 1;
	int32 NumPointsZ = FMath::FloorToInt((Max.Z - Min.Z) / GridSpacing) + 1;
	
	// Ensure we have at least one node  in each dimension
	NumPointsX = FMath::Max(1, NumPointsX);
	NumPointsY = FMath::Max(1, NumPointsY);
	NumPointsZ = FMath::Max(1, NumPointsZ);
	
	// Generate the grid of nodes
	int32 NodeId = 0;
	for (int32 X = 0; X < NumPointsX; X++)
	{
		for (int32 Y = 0; Y < NumPointsY; Y++)
		{
			for (int32 Z = 0; Z < NumPointsZ; Z++)
			{
				// Calculate the position within the bounds
				FVector WorldPosition(
					Min.X + X * GridSpacing,
					Min.Y + Y * GridSpacing,
					Min.Z + Z * GridSpacing
				);
				
				// Only create a node if it's inside the volume
				if (EncompassesPoint(WorldPosition))
				{
					// Create new pathfinding node
					UFlyingPathfindingNode* NewNode = NewObject<UFlyingPathfindingNode>(this);
					NewNode->IdNode = NodeId++;
					AddInstanceComponent(NewNode);
					NewNode->RegisterComponent();
					NewNode->SetWorldLocation(WorldPosition);
					
					// Add to our arrays and maps
					PathfindingNodes.Add(NewNode);
					NodeGrid.Add(FIntVector(X, Y, Z), NewNode);
				}
			}
		}
	}
	
	// Connect neighbors (26 connections, including diagonal)
	TArray<FIntVector> Offsets;
	for (int32 OffsetX = -1; OffsetX <= 1; OffsetX++)
	{
		for (int32 OffsetY = -1; OffsetY <= 1; OffsetY++)
		{
			for (int32 OffsetZ = -1; OffsetZ <= 1; OffsetZ++)
			{
				// Skip the center point
				if (OffsetX == 0 && OffsetY == 0 && OffsetZ == 0)
				{
					continue;
				}
				
				Offsets.Add(FIntVector(OffsetX, OffsetY, OffsetZ));
			}
		}
	}
	
	// Connect each node to its neighbors
	for (auto& Pair : NodeGrid)
	{
		FIntVector GridPos = Pair.Key;
		UFlyingPathfindingNode* CurrentNode = Pair.Value;
		
		if (!CurrentNode)
		{
			continue;
		}
		
		// Initialize the arrays
		CurrentNode->AllConnectedNeighbours.Empty();
		CurrentNode->AllConnectedNeighboursBaseCost.Empty();
		
		// Check all potential neighbors
		for (const FIntVector& Offset : Offsets)
		{
			FIntVector NeighborPos = GridPos + Offset;
			
			UFlyingPathfindingNode* NeighborNode = NodeGrid.FindRef(NeighborPos);
			
			// Only add if the neighbor exists
			if (NeighborNode)
			{
				// Add to connected neighbors
				CurrentNode->AllConnectedNeighbours.Add(NeighborNode);
				
				// Calculate distance cost
				float Cost = FVector(Offset).Size();
				CurrentNode->AllConnectedNeighboursBaseCost.Add(Cost);
			}
		}
	}
}

void AFlyingPathfinderVolume::UpdateDebugVisualization()
{
	// Clear any previous debug drawing
	FlushPersistentDebugLines(GetWorld());
	FlushDebugStrings(GetWorld());
	
	if (bShowDebugSphere)
	{
		for (int32 i=0; i<PathfindingNodes.Num(); i++)
		{
			DrawDebugSphere(
				GetWorld(),
				PathfindingNodes[i]->GetComponentLocation(),
				DebugSphereRadius,
				8,
				DebugSphereColor,
				true,
				-1.0f, 
				0,
				1.0f
			);

			if (i == DebugIndexNode)
			{
				// Draw debug lines between this node and all its connections
				UFlyingPathfindingNode* CurrentNode = PathfindingNodes[i];
				FVector NodeLocation = CurrentNode->GetComponentLocation();
				
				for (UFlyingPathfindingNode* Neighbor : CurrentNode->AllConnectedNeighbours)
				{
					if (Neighbor)
					{
						DrawDebugLine(
							GetWorld(),
							NodeLocation,
							Neighbor->GetComponentLocation(),
							FColor::Yellow,
							true,
							-1.0f,
							0,
							1.0f
						);
					}
				}
			}
		}
	}
}
#endif

// Called every frame
void AFlyingPathfinderVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

