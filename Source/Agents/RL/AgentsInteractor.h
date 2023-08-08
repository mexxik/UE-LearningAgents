// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsInteractor.h"
#include "LearningAgentsObservations.h"
#include "LearningAgentsActions.h"
#include "AgentsInteractor.generated.h"

/**
 * 
 */
UCLASS()
class AGENTS_API UAgentsInteractor : public ULearningAgentsInteractor
{
	GENERATED_BODY()
	
private:
	UFloatAction* ActionForward;
	//UFloatAction* ActionBack;
	UFloatAction* ActionRight;
	UFloatAction* ActionTurn;
	//UFloatAction* ActionLeft;
	UFloatArrayAction* ActionsMove;
	TArray<UFloatObservation *> Observations;

public:
	virtual void SetupActions_Implementation() override;
	virtual void SetupObservations_Implementation() override;
	virtual void SetObservations_Implementation(const TArray<int32>& AgentIds) override;
	virtual void GetActions_Implementation(const TArray<int32>& AgentIds) override;

	virtual void PostInitProperties() override;

	FORCEINLINE void SetManager(ALearningAgentsManager* AManager) { Manager = AManager; }
};
