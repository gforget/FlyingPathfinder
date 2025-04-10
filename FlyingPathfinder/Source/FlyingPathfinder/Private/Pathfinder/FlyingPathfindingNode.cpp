// Copyright(c) 2025 Gabriel Forget. All Rights Reserved.
#include "Pathfinder/FlyingPathfindingNode.h"

// Sets default values for this component's properties
UFlyingPathfindingNode::UFlyingPathfindingNode()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UFlyingPathfindingNode::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UFlyingPathfindingNode::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

