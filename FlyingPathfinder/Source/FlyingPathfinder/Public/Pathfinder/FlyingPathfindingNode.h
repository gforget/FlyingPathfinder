// Copyright(c) 2025 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "FlyingPathfindingNode.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLYINGPATHFINDER_API UFlyingPathfindingNode : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFlyingPathfindingNode();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NodePath Properties")
	int IdNode = -1;
	
	UPROPERTY(BlueprintReadOnly, Category = "NodePath Properties")
	TArray<UFlyingPathfindingNode*> AllNeighboursConnectionInfo;
	
	UPROPERTY(BlueprintReadOnly, Category = "NodePath Properties")
	TArray<UFlyingPathfindingNode*> AllConnectedNeighbours;

	UPROPERTY()
	TArray<float> AllConnectedNeighboursBaseCost;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
};
