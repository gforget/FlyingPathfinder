// Copyright(c) 2025 Gabriel Forget. All Rights Reserved.
#include "Controllers/CustomAIController.h"

#include "Pawns/CustomDefaultPawn.h"
#include "BehaviorTree/BlackboardComponent.h"

// Sets default values
ACustomAIController::ACustomAIController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ACustomAIController::SetPositionToGo(FVector PositionToGo)
{
	GetBlackboardComponent()->SetValueAsVector(TEXT("PositionToGo"), PositionToGo);
}

// Called when the game starts or when spawned
void ACustomAIController::BeginPlay()
{
	Super::BeginPlay();
	Cast<ACustomDefaultPawn>(GetWorld()->GetFirstPlayerController()->GetPawn())->RegisterAIController(this);
}

void ACustomAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (AIBehavior != nullptr)
	{
		RunBehaviorTree(AIBehavior);
	}
}

// Called every frame
void ACustomAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

