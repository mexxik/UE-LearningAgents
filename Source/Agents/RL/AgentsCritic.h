// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsCritic.h"
#include "AgentsCritic.generated.h"

/**
 * 
 */
UCLASS()
class AGENTS_API UAgentsCritic : public ULearningAgentsCritic
{
	GENERATED_BODY()
	
public:
	FORCEINLINE void SetManager(ALearningAgentsManager* AManager) { Manager = AManager; }
};
