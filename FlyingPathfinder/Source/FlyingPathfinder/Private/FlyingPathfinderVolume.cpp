// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingPathfinderVolume.h"
#include "DrawDebugHelpers.h"
#include "Components/BrushComponent.h"


// Sets default values
AFlyingPathfinderVolume::AFlyingPathfinderVolume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFlyingPathfinderVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFlyingPathfinderVolume::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	// Always refresh debug visualization when constructed
	UpdateDebugVisualization();
}

#if WITH_EDITOR
void AFlyingPathfinderVolume::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	// Update debug visualization when any property changes
	UpdateDebugVisualization();
}

void AFlyingPathfinderVolume::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	
	// Update debug visualization after being moved
	UpdateDebugVisualization();
}

void AFlyingPathfinderVolume::UpdateDebugVisualization()
{
	// Clear any previous debug drawing
	FlushPersistentDebugLines(GetWorld());
	FlushDebugStrings(GetWorld());
	
	// Only draw the debug sphere if this volume is selected in the editor
	if (bShowDebugSphere && GetWorld() && GetWorld()->IsEditorWorld() && IsSelectedInEditor())
	{
		// Get the volume's brush component
		UBrushComponent* BrushComponentPtr = GetBrushComponent();
		if (!BrushComponentPtr)
		{
			return;
		}
		
		// Get the bounds of the volume
		FBox Bounds = BrushComponentPtr->Bounds.GetBox();
		
		// Calculate the number of points in each dimension based on the grid spacing
		FVector Min = Bounds.Min;
		FVector Max = Bounds.Max;
		
		// Calculate the number of grid points in each dimension
		int32 NumPointsX = FMath::FloorToInt((Max.X - Min.X) / GridSpacing) + 1;
		int32 NumPointsY = FMath::FloorToInt((Max.Y - Min.Y) / GridSpacing) + 1;
		int32 NumPointsZ = FMath::FloorToInt((Max.Z - Min.Z) / GridSpacing) + 1;
		
		// Ensure we have at least one point in each dimension
		NumPointsX = FMath::Max(1, NumPointsX);
		NumPointsY = FMath::Max(1, NumPointsY);
		NumPointsZ = FMath::Max(1, NumPointsZ);
		
		// Generate the grid of debug spheres
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
					
					// Check if the point is inside the volume
					bool bInsideVolume = true;
					if (bLimitGridToBounds)
					{
						bInsideVolume = EncompassesPoint(WorldPosition);
					}
					
					// Only draw the sphere if it's inside the volume or we're not limiting to bounds
					if (bInsideVolume)
					{
						DrawDebugSphere(
							GetWorld(),
							WorldPosition,
							DebugSphereRadius,
							8, // Segments (reduced for performance with many spheres)
							DebugSphereColor,
							true, // Persistent
							-1.0f, // Lifetime
							0, // Depth priority
							1.0f // Thickness (reduced for cleaner look with multiple spheres)
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

