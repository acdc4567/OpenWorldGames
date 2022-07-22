// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/Enemy.h"
#include "BehaviorTree/BehaviorTree.h"





AEnemyController::AEnemyController() {

	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>("BlackBoardComponent");
	check(BlackboardComponent);

	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComp");
	check(BehaviorTreeComponent);



}

void AEnemyController::OnPossess(APawn* InPawn) {
	Super::OnPossess( InPawn);


	if (InPawn == nullptr)return;
	AEnemy* Enemy = Cast<AEnemy>(InPawn);
	if (Enemy) {

		if (Enemy->GetBehaviorTree()) {

			BlackboardComponent->InitializeBlackboard(*(Enemy->GetBehaviorTree()->BlackboardAsset));

		}

	}








}
