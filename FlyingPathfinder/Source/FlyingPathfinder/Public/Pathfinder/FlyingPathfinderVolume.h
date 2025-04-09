// Fill out your copyright notice in the Description page of Project Settings.

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

	GenericStack<UFlyingPathfindingNode*> GetPathToDestination(UFlyingPathfindingNode* InitialNode, UFlyingPathfindingNode* DestinationNode);
	
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

	// 3D grid to store references to nodes by position
	UPROPERTY()
	TMap<FIntVector, UFlyingPathfindingNode*> NodeGrid;

	//Debug properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	int DebugIndexNode = 0;
	
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShowDebugSphere = true;
	
	UPROPERTY(EditAnywhere, Category = "Debug", meta = (EditCondition = "bShowDebugSphere"))
	float DebugSphereRadius = 20.0f;
	
	UPROPERTY(EditAnywhere, Category = "Debug", meta = (EditCondition = "bShowDebugSphere"))
	FColor DebugSphereColor = FColor::Green;
	
	UPROPERTY(EditAnywhere, Category = "Debug", meta = (EditCondition = "bShowDebugSphere", ClampMin = "50.0"))
	float GridSpacing = 200.0f;

public:
	virtual void Tick(float DeltaTime) override;
};
