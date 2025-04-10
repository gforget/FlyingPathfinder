// Copyright(c) 2025 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FlyingPathfinderGameMode.generated.h"

class AFlyingPathfinderVolume;
/**
 * 
 */
UCLASS()
class FLYINGPATHFINDER_API AFlyingPathfinderGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	TArray<AFlyingPathfinderVolume*> AllFlyingPathfinderVolume;
	
	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
	AFlyingPathfinderVolume* GetVolumeAtLocation(const FVector& WorldLocation) const;
	
protected:
	virtual void BeginPlay() override;
};
