// Copyright(c) 2025 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "CustomAIController.generated.h"

UCLASS()
class FLYINGPATHFINDER_API ACustomAIController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACustomAIController();

	UPROPERTY(EditDefaultsOnly)
	UBehaviorTree* AIBehavior;

	void SetPositionToGo(FVector PositionToGo);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;
	
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
