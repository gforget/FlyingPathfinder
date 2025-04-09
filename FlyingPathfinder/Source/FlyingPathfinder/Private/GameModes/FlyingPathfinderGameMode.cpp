// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/FlyingPathfinderGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "FlyingPathfinder/Public/Pathfinder/FlyingPathfinderVolume.h"

void AFlyingPathfinderGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFlyingPathfinderVolume::StaticClass(), FoundActors);
	
	for (AActor* Actor : FoundActors)
	{
		if (AFlyingPathfinderVolume* Volume = Cast<AFlyingPathfinderVolume>(Actor))
		{
			AllFlyingPathfinderVolume.Add(Volume);
		}
	}
}

AFlyingPathfinderVolume* AFlyingPathfinderGameMode::GetVolumeAtLocation(const FVector& WorldLocation) const
{
	//TODO: Optimization: implement spatial partitioning for large numbers of volumes
	
	for (AFlyingPathfinderVolume* Volume : AllFlyingPathfinderVolume)
	{
		if (Volume && Volume->EncompassesPoint(WorldLocation))
		{
			return Volume;
		}
	}
	
	return nullptr;
}
