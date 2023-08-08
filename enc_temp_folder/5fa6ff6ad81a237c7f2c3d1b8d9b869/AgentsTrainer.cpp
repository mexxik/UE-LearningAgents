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
	UE_LOG(LogTemp, Warning, TEXT("SetupRewards_Implementation"));

	Reward = UFloatReward::AddFloatReward(this, "Reward_1", 1.0);
}

void UAgentsTrainer::SetupCompletions_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("SetupCompletions_Implementation"));

	TargetCompletion = UConditionalCompletion::AddConditionalCompletion(this, "OnTarget", ELearningAgentsCompletion::Truncation);
    //StuckCompletion = UConditionalCompletion::AddConditionalCompletion(this, "Stuck", ELearningAgentsCompletion::Truncation);
	// ExpireCompletion = UConditionalCompletion::AddConditionalCompletion(this, "Expire", ELearningAgentsCompletion::Truncation);
}

void UAgentsTrainer::SetCompletions_Implementation(const TArray<int32>& AgentIds)
{
	// UE_LOG(LogTemp, Warning, TEXT("SetCompletions_Implementation"));

	for (int32 AgentId : AgentIds)
	{
		AAgentsCharacter* Agent = Cast<AAgentsCharacter>(Manager->GetAgent(AgentId));

		TargetCompletion->SetConditionalCompletion(AgentId, Agent->IsOnTarget());
        //StuckCompletion->SetConditionalCompletion(AgentId, Agent->IsStuck());
		// ExpireCompletion->SetConditionalCompletion(AgentId, Steps > EpisodeLength);
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

        // Increment the number of steps the agent has taken in the episode
        Agent->StepsInEpisode++;

        float RewardValue = 0.0f;

        if (Agent->IsOnTarget())
        {
            /*if (Agent->ReachedTarget())
            {
                RewardValue = 0.0f;
            }
            else
            {
                RewardValue = 100.0f;
				Agent->SetReachedTarget(true);
            }*/
            RewardValue = 100.0f;
            // Provide a high reward for reaching the target
            
            // UE_LOG(LogTemp, Warning, TEXT("Agent on target, reward: %f"), RewardValue);
        }
        else
        {
            // Penalize the agent for hitting walls
           /* if (Agent->IsHitWall())
            {
                RewardValue -= 2.0f;
            }*/

            float DistanceFromRespawn = Agent->GetDistanceFromRespawn();
            float LocationTravelled = Agent->GetLocationTravelledAfterRespawn();

            //float DistanceFromRespawnDelta = Agent->GetPreviousDistanceFromRespawnDelta();
            //float DistanceFromRespawn = Agent->GetDistanceFromRespawn() / 1000.0f;
            if (AgentId == 0)
            {
                 //UE_LOG(LogTemp, Warning, TEXT("DistanceFromRespawn: %f, LocationTravelled: %f"), 
                 //    DistanceFromRespawn, LocationTravelled);
            }

            //if (DistanceFromRespawnDelta <= 0.0)
            //{
                //RewardValue -= 0.05f;
            //}

            float DistanceToTarget = FVector::Dist(Agent->GetActorLocation(), Target->GetActorLocation());
            //float WallPenalty = Agent->IsHitWall() ? 20.0f : 0.0f;

            //RewardValue -= WallPenalty;

            if (Agent->GetPreviousDistanceToTarget() - DistanceToTarget > 2.0f ) {
                //RewardValue += 0.1f; // Reward for moving closer
            }
            else {
                //RewardValue -= 0.2f; // Small penalty for moving away
            }

            RewardValue -= 0.1f;

            Agent->UpdatePreviousDistanceToTarget(DistanceToTarget);

            // RewardValue += 0.0001f * DistanceFromRespawn;
            //RewardValue += 0.0001f * LocationTravelled;


            if (false && Target)
            {
                //float DistanceToTarget = FVector::Dist(Agent->GetActorLocation(), Target->GetActorLocation());
                //float PreviousDistanceToTarget = Agent->GetPreviousDistanceToTarget();
               // float DistanceToTargetDelta = DistanceToTarget - PreviousDistanceToTarget;

                //float DistanceFromRespawn = Agent->GetDistanceFromRespawn() / 1000.0f;

                // UE_LOG(LogTemp, Warning, TEXT("D: %f, P: %f, DDelta: %f"), DistanceToTarget, PreviousDistanceToTarget, DistanceToTargetDelta);

                // Adjust target and respawn penalties based on distances
                float TargetPenalty = FMath::Max(0.0f, 5.0f - DistanceToTarget);
                // float SpawnPenalty = FMath::Max(0.0f, 10.0f - DistanceFromRespawn);
                //float WallPenalty = Agent->IsHitWall() ? 2.0f : 0.0f;
                float StuckPenalty = Agent->IsStuck() ? 20.0f : 0.0f;


                // Penalize for moving away from the target
                //if (DistanceToTarget < Agent->GetPreviousDistanceToTarget()) {
                //    RewardValue += 2.0f; // Reward for moving closer
                //}
                //else {
                //    RewardValue -= 1.0f; // Small penalty for moving away
                //}

                // Penalize for moving in the same direction
                if (Agent->GetDirectionOfMovement() == Agent->GetPreviousDirectionOfMovement()) {
                    //RewardValue -= 0.5f; // Small penalty for moving in the same direction
                }

                // Combine the penalties and rewards
                //RewardValue -= TargetPenalty;
                // RewardValue -= SpawnPenalty;
                //RewardValue -= DistanceToTargetDelta;
                //RewardValue -= WallPenalty;
                //RewardValue -= StuckPenalty;

                RewardValue -= 0.1f;

                // Update previous distance and direction
                Agent->UpdatePreviousDistanceToTarget(DistanceToTarget);
                FVector CurrentDirectionOfMovement = (Target->GetActorLocation() - Agent->GetActorLocation()).GetSafeNormal();
                Agent->UpdateDirectionOfMovement(CurrentDirectionOfMovement);

                if (AgentId == 0)
                {
                    //UE_LOG(LogTemp, Warning, TEXT("TargetPenalty: %f, SpawnPenalty: %f, WallPenalty: %f, RewardValue: %f"),
                    //    TargetPenalty, SpawnPenalty, WallPenalty, RewardValue);
                }
            }
        }

        // Set the reward for the current agent
        Reward->SetFloatReward(AgentId, RewardValue);

        Agent->AddReward(RewardValue);
        Agent->SetPreviousLocation(Agent->GetActorLocation());
    }
}

void UAgentsTrainer::ResetEpisodes_Implementation(const TArray<int32>& AgentIds)
{
    // UE_LOG(LogTemp, Warning, TEXT("ResetEpisodes_Implementation, Num: %d"), AgentIds.Num());

	for (int32 AgentId : AgentIds)
	{
		AAgentsCharacter* Agent = Cast<AAgentsCharacter>(Manager->GetAgent(AgentId));
		Agent->StepsInEpisode = 0;

        int32 RespawnId = Steps / 200000 + 1;
        if (RespawnId > 3) {
			RespawnId = 3;
		}

        Agent->SetRespawnId(RespawnId);
		Agent->Respawn();
        Agent->ResetReward();
    }

    // Steps = 0;
}
