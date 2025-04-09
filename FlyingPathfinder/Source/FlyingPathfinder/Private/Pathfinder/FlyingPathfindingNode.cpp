// Fill out your copyright notice in the Description page of Project Settings.
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

