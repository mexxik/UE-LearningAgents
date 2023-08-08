// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsPolicy.h"
#include "AgentsPolicy.generated.h"

/**
 * 
 */
UCLASS()
class AGENTS_API UAgentsPolicy : public ULearningAgentsPolicy
{
	GENERATED_BODY()
	
public:
	FORCEINLINE void SetManager(ALearningAgentsManager* AManager) { Manager = AManager; }
};
