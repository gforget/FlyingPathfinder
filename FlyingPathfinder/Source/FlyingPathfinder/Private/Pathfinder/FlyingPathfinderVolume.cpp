// Copyright(c) 2025 Gabriel Forget. All Rights Reserved.

#include "Pathfinder/FlyingPathfinderVolume.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "Components/BrushComponent.h"
#include "Utility/GenericPriorityQueue.h"

// Sets default values
AFlyingPathfinderVolume::AFlyingPathfinderVolume()
{
	PrimaryActorTick.bCanEverTick = false;
}

GenericStack<UFlyingPathfindingNode*> AFlyingPathfinderVolume::GetPathToDestination(FVector InitialPoint, FVector DestinationPoint)
{
	// Clear any existing temporary nodes
	ClearTemporaryNodes();

	//TODO: Slope can still be a problem when trying to connect to the grid. Need something to offset depending on the slope of the floor
	// Create temporary nodes at the initial and destination positions
	UFlyingPathfindingNode* InitialNode = CreateTemporaryNode(InitialPoint + FVector(0.0f, 0.0f, NeighbourConnectionTraceRadius+10.0f));
	UFlyingPathfindingNode* DestinationNode = CreateTemporaryNode(DestinationPoint + FVector(0.0f, 0.0f, NeighbourConnectionTraceRadius+10.0f));
	
	GenericStack<UFlyingPathfindingNode*> PathStack;
	if (!InitialNode || !DestinationNode)
	{
		DebugScreen("Node parameters send to pathfinding are empty", FColor::Red);
		return PathStack;
	}
	
	GenericPriorityQueue<UFlyingPathfindingNode*, float> frontier;
	frontier.Enqueue(InitialNode, 0.0f);

	TMap<int, UFlyingPathfindingNode*> came_from;
	AddNodeToCameFrom(came_from, InitialNode->IdNode, nullptr);
	
	TMap<int, float> cost_so_far;
	AddCostToCostSoFar(cost_so_far, InitialNode->IdNode, 0.0f);

	bool bPathFound = false;
	int iteration = 0;

	while (!frontier.IsEmpty())
	{
		UFlyingPathfindingNode* current = frontier.Dequeue();
		for (int i = 0; i<current->AllConnectedNeighbours.Num(); i++)
		{
			iteration++;
			UFlyingPathfindingNode* next = current->AllConnectedNeighbours[i];
			if (!next)
			{
				DebugScreen("Current node"+ current->GetName() +" neighbour " + FString::FromInt(i) + "is Null", FColor::Red);
				continue;
			}
			
			const float new_cost = GetCostFromCostSoFar(cost_so_far, current->IdNode) + current->AllConnectedNeighboursBaseCost[i]*next->WeightCost;
			
			if ((!cost_so_far.Contains(next->IdNode) || new_cost < GetCostFromCostSoFar(cost_so_far, next->IdNode)))
			{
				AddCostToCostSoFar(cost_so_far, next->IdNode, new_cost);
				AddNodeToCameFrom(came_from, next->IdNode, current);
				
				frontier.Enqueue(next, new_cost + Heuristic(DestinationNode->GetComponentLocation(), next->GetComponentLocation()));
				if (next->IdNode == DestinationNode->IdNode)
				{
					bPathFound = true;
					break;
				}
			}
		}

		if (bPathFound)
		{
			break;
		}
	}
	
	//Generate the path
	if (bPathFound)
	{
		UFlyingPathfindingNode* current = DestinationNode;
		while(current->IdNode != InitialNode->IdNode)
		{
			PathStack.Push(current);
			current = GetNodeFromCameFrom(came_from, current->IdNode);
		}
	}
	
	// clear memory to avoid memory leak
	frontier.Clear(); 
	came_from.Empty();
	cost_so_far.Empty();

	if (bShowDebugPath)
	{
		FlushPersistentDebugLines(GetWorld());
		FlushDebugStrings(GetWorld());
		GenericStack<UFlyingPathfindingNode*> PathStack2 = PathStack;
					
		UFlyingPathfindingNode* PrevNode = nullptr;
		for (int32 i=PathStack2.Num()-1; i>=0; i--)
		{
			UFlyingPathfindingNode* CurrentNode = PathStack2.Pop();
			DrawDebugSphere(
				GetWorld(),
				CurrentNode->GetComponentLocation(),
				20.0f,
				8,
				FColor::Yellow,
				true,
				-1.0f, 
				0,
				1.0f
			);
						
			DrawDebugLine(
				GetWorld(),
				PrevNode ? PrevNode->GetComponentLocation() : InitialNode->GetComponentLocation(),
				CurrentNode->GetComponentLocation(),
				FColor::Blue,
				true,
				-1.0f,
				0,
				1.0f
			);
					
			PrevNode = CurrentNode;
		}
	}
	
	return PathStack;
}

void AFlyingPathfinderVolume::AddNodeToCameFrom(TMap<int, UFlyingPathfindingNode*>& came_from, int IdNode,
	UFlyingPathfindingNode* ValueNode)
{
	if (!came_from.Contains(IdNode))
	{
		came_from.Add(IdNode, ValueNode);
	}
	else
	{
		came_from[IdNode] = ValueNode;
	}
}

UFlyingPathfindingNode* AFlyingPathfinderVolume::GetNodeFromCameFrom(TMap<int, UFlyingPathfindingNode*>& came_from, int IdNode)
{
	if (came_from.Contains(IdNode))
	{
		return came_from[IdNode];
	}

	return nullptr;
}

void AFlyingPathfinderVolume::AddCostToCostSoFar(TMap<int, float>& cost_so_far, int IdNode, float Cost)
{
	if (!cost_so_far.Contains(IdNode))
	{
		cost_so_far.Add(IdNode, Cost);
	}
	else
	{
		cost_so_far[IdNode] = Cost;
	}
}

float AFlyingPathfinderVolume::GetCostFromCostSoFar(TMap<int, float>& cost_so_far, int IdNode)
{
	if (cost_so_far.Contains(IdNode))
	{
		return cost_so_far[IdNode];
	}

	return 0.0f;
}

UFlyingPathfindingNode* AFlyingPathfinderVolume::CreateTemporaryNode(const FVector& Location)
{
	// Create a new pathfinding node
	UFlyingPathfindingNode* TempNode = NewObject<UFlyingPathfindingNode>(this);
	TempNode->IdNode = -100 - TemporaryNodes.Num(); // Use negative IDs for temporary nodes
	AddInstanceComponent(TempNode);
	TempNode->RegisterComponent();
	TempNode->SetWorldLocation(Location);
	
	// Add to temporary nodes array
	TemporaryNodes.Add(TempNode);
	
	// Initialize connection arrays
	TempNode->AllConnectedNeighbours.Empty();
	TempNode->AllConnectedNeighboursBaseCost.Empty();
	
	// Get the volume bounds
	UBrushComponent* BrushComponentPtr = GetBrushComponent();
	if (!BrushComponentPtr)
	{
		return TempNode;
	}
	
	FBox Bounds = BrushComponentPtr->Bounds.GetBox();
	FVector Min = Bounds.Min;
	
	// Convert world position to grid position (used for optimisation search)
	FIntVector GridPos(
		FMath::RoundToInt((Location.X - Min.X) / GridSpacing.X),
		FMath::RoundToInt((Location.Y - Min.Y) / GridSpacing.Y),
		FMath::RoundToInt((Location.Z - Min.Z) / GridSpacing.Z)
	);
	
	// Start with radius 1 and expand until connections are found
	int32 SearchRadius = 1;
	bool bFoundConnections = false;
	const int32 MaxSearchRadius = 10;
	
	while (!bFoundConnections && SearchRadius <= MaxSearchRadius)
	{
		// Get nodes in the current radius
		TArray<UFlyingPathfindingNode*> NodesInRadius = FindNodesInRadius(Location, SearchRadius);
		
		// Connect to nodes in this radius
		for (UFlyingPathfindingNode* PermanentNode : NodesInRadius)
		{
			float Distance = FVector::Dist(Location, PermanentNode->GetComponentLocation());
			
			// Check line of sight before connecting
			if (HasClearSphereLineOfSight(Location, PermanentNode->GetComponentLocation()))
			{
				// Connect temporary node to permanent node
				TempNode->AllConnectedNeighbours.Add(PermanentNode);
				TempNode->AllConnectedNeighboursBaseCost.Add(Distance);
				
				// Connect permanent node to temporary node
				PermanentNode->AllConnectedNeighbours.Add(TempNode);
				PermanentNode->AllConnectedNeighboursBaseCost.Add(Distance);
				
				bFoundConnections = true;
			}
		}
		
		// Increase radius if no connections found
		if (!bFoundConnections)
		{
			SearchRadius++;
		}
	}

	//TODO: Might have to return to this. Connecting temporary node isn't usefull in the context of the tool, but might
	//be helpfull later.
	
	// Connect to other temporary nodes using the final search radius
	// for (UFlyingPathfindingNode* OtherTempNode : TemporaryNodes)
	// {
	// 	// Skip self
	// 	if (OtherTempNode == TempNode)
	// 	{
	// 		continue;
	// 	}
	// 	
	// 	// Calculate grid distance between nodes
	// 	FVector OtherLocation = OtherTempNode->GetComponentLocation();
	// 	FIntVector OtherGridPos(
	// 		FMath::RoundToInt((OtherLocation.X - Min.X) / GridSpacing.X),
	// 		FMath::RoundToInt((OtherLocation.Y - Min.Y) / GridSpacing.Y),
	// 		FMath::RoundToInt((OtherLocation.Z - Min.Z) / GridSpacing.Z)
	// 	);
	// 	
	// 	FIntVector GridDiff = GridPos - OtherGridPos;
	// 	
	// 	// Only connect if within the expanded search radius
	// 	if (FMath::Abs(GridDiff.X) <= SearchRadius && 
	// 		FMath::Abs(GridDiff.Y) <= SearchRadius && 
	// 		FMath::Abs(GridDiff.Z) <= SearchRadius)
	// 	{
	// 		float Distance = FVector::Dist(Location, OtherLocation);
	// 		
	// 		// Check line of sight before connecting
	// 		if (HasClearSphereLineOfSight(Location, OtherLocation))
	// 		{
	// 			// Connect this temp node to other temp node
	// 			TempNode->AllConnectedNeighbours.Add(OtherTempNode);
	// 			TempNode->AllConnectedNeighboursBaseCost.Add(Distance);
	// 			
	// 			// Connect other temp node to this temp node
	// 			OtherTempNode->AllConnectedNeighbours.Add(TempNode);
	// 			OtherTempNode->AllConnectedNeighboursBaseCost.Add(Distance);
	// 		}
	// 	}
	// }
	
	// Add debug info if we reached the max search radius
	if (!bFoundConnections)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateTemporaryNode: No connections found within maximum search radius for node at %s"), *Location.ToString());
	}
	
	return TempNode;
}

void AFlyingPathfinderVolume::ClearTemporaryNodes()
{
	// Remove connections from permanent nodes to temporary nodes
	for (UFlyingPathfindingNode* PermanentNode : PathfindingNodes)
	{
		TArray<UFlyingPathfindingNode*> RemainingNeighbors;
		TArray<float> RemainingCosts;
		
		// Keep only connections to permanent nodes
		for (int32 i = 0; i < PermanentNode->AllConnectedNeighbours.Num(); i++)
		{
			UFlyingPathfindingNode* Neighbor = PermanentNode->AllConnectedNeighbours[i];
			if (!TemporaryNodes.Contains(Neighbor))
			{
				RemainingNeighbors.Add(Neighbor);
				RemainingCosts.Add(PermanentNode->AllConnectedNeighboursBaseCost[i]);
			}
		}
		
		// Update the arrays
		PermanentNode->AllConnectedNeighbours = RemainingNeighbors;
		PermanentNode->AllConnectedNeighboursBaseCost = RemainingCosts;
	}
	
	// Destroy temporary nodes
	for (UFlyingPathfindingNode* TempNode : TemporaryNodes)
	{
		if (TempNode)
		{
			TempNode->DestroyComponent();
		}
	}
	
	// Clear the array
	TemporaryNodes.Empty();
}

UFlyingPathfindingNode* AFlyingPathfinderVolume::FindClosestNode(const FVector& Point)
{
	if (PathfindingNodes.Num() == 0)
	{
		return nullptr;
	}

	// Get the volume bounds
	UBrushComponent* BrushComponentPtr = GetBrushComponent();
	if (!BrushComponentPtr)
	{
		return nullptr;
	}
	
	FBox Bounds = BrushComponentPtr->Bounds.GetBox();
	FVector Min = Bounds.Min;

	// Convert world position to grid position (used for optimisation search)
	FIntVector GridPos(
		FMath::RoundToInt((Point.X - Min.X) / GridSpacing.X),
		FMath::RoundToInt((Point.Y - Min.Y) / GridSpacing.Y),
		FMath::RoundToInt((Point.Z - Min.Z) / GridSpacing.Z)
	);

	// Try to find the exact node first
	if (UFlyingPathfindingNode** ExactNode = NodeGrid.Find(GridPos))
	{
		return *ExactNode;
	}

	// If not found, search for the closest node
	UFlyingPathfindingNode* ClosestNode = nullptr;
	float ClosestDistSq = MAX_FLT;

	// Search in a growing cube pattern from the grid position
	for (int32 Radius = 1; Radius < 10; Radius++)
	{
		bool bFoundNode = false;

		// Check all node at this radius from the center
		for (int32 X = -Radius; X <= Radius; X++)
		{
			for (int32 Y = -Radius; Y <= Radius; Y++)
			{
				for (int32 Z = -Radius; Z <= Radius; Z++)
				{
					// Only check node on the surface of the grid cube (not inside, for it already has been analysed)
					if (FMath::Abs(X) != Radius && FMath::Abs(Y) != Radius && FMath::Abs(Z) != Radius)
					{
						continue;
					}

					FIntVector NeighborPos = GridPos + FIntVector(X, Y, Z);
					if (UFlyingPathfindingNode** NodePtr = NodeGrid.Find(NeighborPos))
					{
						UFlyingPathfindingNode* Node = *NodePtr;
						float DistSq = FVector::DistSquared(Point, Node->GetComponentLocation());
						
						if (DistSq < ClosestDistSq)
						{
							ClosestDistSq = DistSq;
							ClosestNode = Node;
							bFoundNode = true;
						}
					}
				}
			}
		}

		// If we found at least one node at this radius, return the closest one
		if (bFoundNode)
		{
			return ClosestNode;
		}
	}

	// If no node was found in the cube search, fall back to a full search
	for (UFlyingPathfindingNode* Node : PathfindingNodes)
	{
		float DistSq = FVector::DistSquared(Point, Node->GetComponentLocation());
		if (DistSq < ClosestDistSq)
		{
			ClosestDistSq = DistSq;
			ClosestNode = Node;
		}
	}

	return ClosestNode;
}

TArray<UFlyingPathfindingNode*> AFlyingPathfinderVolume::FindNodesInRadius(const FVector& Point, int32 Radius)
{
	TArray<UFlyingPathfindingNode*> NodesInRadius;
	
	if (PathfindingNodes.Num() == 0)
	{
		return NodesInRadius;
	}

	// Get the volume bounds
	UBrushComponent* BrushComponentPtr = GetBrushComponent();
	if (!BrushComponentPtr)
	{
		return NodesInRadius;
	}
	
	FBox Bounds = BrushComponentPtr->Bounds.GetBox();
	FVector Min = Bounds.Min;

	// Convert world position to grid position (used for optimisation search)
	FIntVector GridPos(
		FMath::RoundToInt((Point.X - Min.X) / GridSpacing.X),
		FMath::RoundToInt((Point.Y - Min.Y) / GridSpacing.Y),
		FMath::RoundToInt((Point.Z - Min.Z) / GridSpacing.Z)
	);

	// Check all nodes within the specified radius
	for (int32 X = -Radius; X <= Radius; X++)
	{
		for (int32 Y = -Radius; Y <= Radius; Y++)
		{
			for (int32 Z = -Radius; Z <= Radius; Z++)
			{
				FIntVector NeighborPos = GridPos + FIntVector(X, Y, Z);
				
				if (UFlyingPathfindingNode** NodePtr = NodeGrid.Find(NeighborPos))
				{
					UFlyingPathfindingNode* Node = *NodePtr;
					NodesInRadius.Add(Node);
				}
			}
		}
	}

	return NodesInRadius;
}

// Called when the game starts or when spawned
void AFlyingPathfinderVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFlyingPathfinderVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#if WITH_EDITOR
void AFlyingPathfinderVolume::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UpdateDebugVisualization();
}

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
	int32 NumPointsX = FMath::FloorToInt((Max.X - Min.X) / GridSpacing.X) + 1;
	int32 NumPointsY = FMath::FloorToInt((Max.Y - Min.Y) / GridSpacing.Y) + 1;
	int32 NumPointsZ = FMath::FloorToInt((Max.Z - Min.Z) / GridSpacing.Z) + 1;
	
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
					Min.X + X * GridSpacing.X,
					Min.Y + Y * GridSpacing.Y,
					Min.Z + Z * GridSpacing.Z
				);
				
				// Only create a node if it's inside the volume and not colliding with any solid surface
				if (EncompassesPoint(WorldPosition) && !IsPositionBlocked(WorldPosition, BlockCheckRadius))
				{
					// Create new pathfinding node
					UFlyingPathfindingNode* NewNode = NewObject<UFlyingPathfindingNode>(this);
					NewNode->IdNode = NodeId++;
					AddInstanceComponent(NewNode);
					NewNode->RegisterComponent();
					NewNode->SetWorldLocation(WorldPosition);
					
					// Add to our arrays
					PathfindingNodes.Add(NewNode);

					// Add to the maps
					//the X,Y,Z position are from the grid metric, not world metric
					//useful for optimisation search later
					NodeGrid.Add(FIntVector(X, Y, Z), NewNode); 
				}
			}
		}
	}
	
	// Connect neighbors (26 maximum connections, including diagonal)
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
			FIntVector NeighborGridPos = GridPos + Offset;
			
			UFlyingPathfindingNode* NeighborNode = NodeGrid.FindRef(NeighborGridPos);
			
			// Only add if the neighbor exists
			if (NeighborNode)
			{
				// Check line of sight between nodes
				FVector CurrentPos = CurrentNode->GetComponentLocation();
				FVector NeighborWorldPos = NeighborNode->GetComponentLocation();
				
				if (HasClearSphereLineOfSight(CurrentPos, NeighborWorldPos))
				{
					// Add to connected neighbors
					CurrentNode->AllConnectedNeighbours.Add(NeighborNode);
					
					// Calculate distance cost
					float Cost = FVector::Dist(CurrentPos, NeighborWorldPos);
					CurrentNode->AllConnectedNeighboursBaseCost.Add(Cost);
				}
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

bool AFlyingPathfinderVolume::IsPositionBlocked(const FVector& Position, float Radius)
{
	if (!GetWorld())
	{
		return false;
	}
	
	// Set up collision parameters
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); // Ignore self
	
	// Ignore pawns (a Character is a pawn, so it is directly included when only checking for Pawn)
	TArray<AActor*> IgnoredActors;
	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		CollisionParams.AddIgnoredActor(*It);
	}
	
	// Set up collision shape
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(Radius);
	
	// Check for collision with static objects only (buildings, terrain, etc.)
	return GetWorld()->OverlapBlockingTestByChannel(
		Position,
		FQuat::Identity,
		ECC_WorldStatic,
		SphereShape,
		CollisionParams
	);
}

bool AFlyingPathfinderVolume::HasClearSphereLineOfSight(const FVector& Start, const FVector& End)
{
	if (!GetWorld())
	{
		return true; // If no world, assume clear path
	}
	
	// Set up collision parameters
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); // Ignore self
	
	// Ignore pawns (a Character is a pawn, so it is directly included when only checking for Pawn)
	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		CollisionParams.AddIgnoredActor(*It);
	}
	
	// Use a sphere trace for better detection of obstacles
	FHitResult HitResult;
	float SphereRadius = NeighbourConnectionTraceRadius;
	
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ECC_WorldStatic,
		FCollisionShape::MakeSphere(SphereRadius),
		CollisionParams
	);
	
	// Return true if no hit (clear path)
	return !bHit;
}

// A* optimization to find the destination node quicker. Only return the most PROBABLE shortest path. Adjust heuristic later depending on situation
float AFlyingPathfinderVolume::Heuristic(FVector goal, FVector next)
{
	return FMath::Abs(goal.X - next.X) + FMath::Abs(goal.Y - next.Y) + FMath::Abs(goal.Z - next.Z);
}
