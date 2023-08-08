// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsTrainer.h"
#include "AgentsTrainer.generated.h"

class UConditionalCompletion;
class UFloatReward;
/**
 * 
 */
UCLASS()
class AGENTS_API UAgentsTrainer : public ULearningAgentsTrainer
{
	GENERATED_BODY()
	
private:
	UConditionalCompletion* TargetCompletion;
	UConditionalCompletion* StuckCompletion;
	UConditionalCompletion* ExpireCompletion;
	UFloatReward* Reward;

	int32 Steps = 0;
	//int32 EpisodeLength = 1000;

public:
	virtual void SetupRewards_Implementation() override;
	virtual void SetupCompletions_Implementation() override;
	virtual void SetCompletions_Implementation(const TArray<int32>& AgentIds) override;
	virtual void SetRewards_Implementation(const TArray<int32>& AgentIds) override;
	virtual void ResetEpisodes_Implementation(const TArray<int32>& AgentIds) override;

	FORCEINLINE void SetManager(ALearningAgentsManager* AManager) { Manager = AManager; }
};
