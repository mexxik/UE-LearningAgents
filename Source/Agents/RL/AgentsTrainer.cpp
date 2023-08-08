// Fill out your copyright notice in the Description page of Project Settings.


#include "AgentsTrainer.h"

#include "LearningAgentsRewards.h"
#include "LearningAgentsCompletions.h"
#include "Agents/AgentsCharacter.h"
#include "LearningAgentsManager.h"
#include "Agents/AgentsTarget.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h"

void UAgentsTrainer::SetupRewards_Implementation()
{
	Reward = UFloatReward::AddFloatReward(this, "Reward_1", 1.0);
}

void UAgentsTrainer::SetupCompletions_Implementation()
{
	TargetCompletion = UConditionalCompletion::AddConditionalCompletion(this, "OnTarget", ELearningAgentsCompletion::Truncation);

}

void UAgentsTrainer::SetCompletions_Implementation(const TArray<int32>& AgentIds)
{
	for (int32 AgentId : AgentIds)
	{
		AAgentsCharacter* Agent = Cast<AAgentsCharacter>(Manager->GetAgent(AgentId));

		TargetCompletion->SetConditionalCompletion(AgentId, Agent->IsOnTarget());
	}

    Steps++;
}

void UAgentsTrainer::SetRewards_Implementation(const TArray<int32>& AgentIds)
{
    AAgentsTarget* Target = nullptr;
    UWorld* World = GetWorld();

    if (World != nullptr)
    {
        for (TActorIterator<AAgentsTarget> It(World); It; ++It)
        {
            Target = *It;
            break;
        }
    }

    for (int32 AgentId : AgentIds)
    {
        AAgentsCharacter* Agent = Cast<AAgentsCharacter>(Manager->GetAgent(AgentId));

        float RewardValue = 0.0f;

        if (Agent->IsOnTarget())
        {
            RewardValue = 100.0f;

        }
        else
        {
            RewardValue -= 0.1f;

        }

        Reward->SetFloatReward(AgentId, RewardValue);

        Agent->AddReward(RewardValue);
    }
}

void UAgentsTrainer::ResetEpisodes_Implementation(const TArray<int32>& AgentIds)
{

	for (int32 AgentId : AgentIds)
	{
		AAgentsCharacter* Agent = Cast<AAgentsCharacter>(Manager->GetAgent(AgentId));

        int32 RespawnId = Steps / 200000 + 1;
        if (RespawnId > 3) {
			RespawnId = 3;
		}

        Agent->SetRespawnId(RespawnId);
		Agent->Respawn();
        Agent->ResetReward();
    }
}
