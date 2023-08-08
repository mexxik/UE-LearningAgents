// Fill out your copyright notice in the Description page of Project Settings.


#include "AgentsInteractor.h"

#include "LearningAgentsManager.h"
#include "InputActionValue.h"
#include "AgentsManager.h"
#include "Agents/AgentsCharacter.h"

void UAgentsInteractor::SetupActions_Implementation()
{
	ActionForward = UFloatAction::AddFloatAction(this, "Forward", 1.0f);
	ActionRight = UFloatAction::AddFloatAction(this, "Right", 1.0f);
	ActionTurn = UFloatAction::AddFloatAction(this, "Turn", 1.0f);
}

void UAgentsInteractor::SetupObservations_Implementation()
{
	Observations.Add(UFloatObservation::AddFloatObservation(this, "Observation_Location_X", 1.0));
	Observations.Add(UFloatObservation::AddFloatObservation(this, "Observation_Location_Y", 1.0));
	Observations.Add(UFloatObservation::AddFloatObservation(this, "Observation_Rotation_Z", 1.0));
}

void UAgentsInteractor::SetObservations_Implementation(const TArray<int32>& AgentIds)
{
	for (int32 AgentId : AgentIds)
	{
		AAgentsCharacter* Agent = Cast<AAgentsCharacter>(Manager->GetAgent(AgentId));
		TArray<float> AgentObservations = Agent->GetObservations();

		if (AgentObservations.Num() == Observations.Num())
		{
			int32 i = 0;
			for (UFloatObservation* Observation : Observations)
			{
				Observation->SetFloatObservation(AgentId, AgentObservations[i]);
				i++;
			}
		}
	}
}

void UAgentsInteractor::GetActions_Implementation(const TArray<int32>& AgentIds)
{
	for (int32 AgentId : AgentIds)
	{
		float ForwardValue = ActionForward->GetFloatAction(AgentId);
		float RightValue = ActionRight->GetFloatAction(AgentId);
		float TurnValue = ActionTurn->GetFloatAction(AgentId);

		AAgentsCharacter* Agent = Cast<AAgentsCharacter>(Manager->GetAgent(AgentId));

		RightValue /= 5.0f;

		FVector2D MovementVector(RightValue, ForwardValue < 0.0f ? -0.2f : 1.0f);
		FInputActionValue MovementInputActionValue(MovementVector);

		Agent->Move(MovementInputActionValue);
		Agent->Turn(TurnValue);

		
		
	}
	// UE_LOG(LogTemp, Warning, TEXT("---------------------------------"));
}

void UAgentsInteractor::PostInitProperties()
{
	Super::PostInitProperties();
}
