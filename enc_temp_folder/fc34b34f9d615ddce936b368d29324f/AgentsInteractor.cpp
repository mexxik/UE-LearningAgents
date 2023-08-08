// Fill out your copyright notice in the Description page of Project Settings.


#include "AgentsInteractor.h"

#include "LearningAgentsManager.h"
#include "InputActionValue.h"
#include "AgentsManager.h"
#include "Agents/AgentsCharacter.h"

void UAgentsInteractor::SetupActions_Implementation()
{
	// UE_LOG(LogTemp, Warning, TEXT("SetupActions_Implementation"));

	//ActionForward = UFloatAction::AddFloatAction(this, "Forward", 1.0f);
	//ActionBack = UFloatAction::AddFloatAction(this, "Back", 0.1f);

	ActionForward = UFloatAction::AddFloatAction(this, "Forward", 1.0f);
	//ActionBack = UFloatAction::AddFloatAction(this, "Back", 1.0f);
	ActionRight = UFloatAction::AddFloatAction(this, "Right", 1.0f);
	ActionTurn = UFloatAction::AddFloatAction(this, "Turn", 1.0f);
	//ActionLeft = UFloatAction::AddFloatAction(this, "Left", 1.0f);

	//ActionsMove = UFloatArrayAction::AddFloatArrayAction(this, "Move", 4, 1.0f);
	
	//ActionRight = UFloatAction::AddFloatAction(this, "Right", 0.001f);
	//ActionLeft = UFloatAction::AddFloatAction(this, "Left", 0.001f);
}

void UAgentsInteractor::SetupObservations_Implementation()
{
	// UE_LOG(LogTemp, Warning, TEXT("SetupObservations_Implementation"));

	//Observations.Add(UFloatObservation::AddFloatObservation(this, "Observation_Spawn_X", 1.0));
	//Observations.Add(UFloatObservation::AddFloatObservation(this, "Observation_Spawn_Y", 1.0));
	//Observations.Add(UFloatObservation::AddFloatObservation(this, "Observation_Spawn_Z", 1.0));
	Observations.Add(UFloatObservation::AddFloatObservation(this, "Observation_Location_X", 1.0));
	Observations.Add(UFloatObservation::AddFloatObservation(this, "Observation_Location_Y", 1.0));
	//Observations.Add(UFloatObservation::AddFloatObservation(this, "Observation_Location_Z", 1.0));
	//Observations.Add(UFloatObservation::AddFloatObservation(this, "Observation_Rotation_X", 1.0));
	//Observations.Add(UFloatObservation::AddFloatObservation(this, "Observation_Rotation_Y", 1.0));
	Observations.Add(UFloatObservation::AddFloatObservation(this, "Observation_Rotation_Z", 1.0));
}

void UAgentsInteractor::SetObservations_Implementation(const TArray<int32>& AgentIds)
{
	// UE_LOG(LogTemp, Warning, TEXT("SetObservations_Implementation"));

	// AAgentsManager* AgentsManager = Cast<AAgentsManager>(Manager);
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
		
		// UFloatObservation::SetFloatObservation(this, AgentId, "Observation_Location_X", Observations[0]);

		// ULearningAgentsObservations::SetFloatObservation(this, AgentId, "Observation_1", 1.0);
	}
}

void UAgentsInteractor::GetActions_Implementation(const TArray<int32>& AgentIds)
{
	// UE_LOG(LogTemp, Warning, TEXT("GetActions_Implementation"));

	for (int32 AgentId : AgentIds)
	{
		float ForwardValue = ActionForward->GetFloatAction(AgentId);
		float RightValue = ActionRight->GetFloatAction(AgentId);
		float TurnValue = ActionTurn->GetFloatAction(AgentId);

		AAgentsCharacter* Agent = Cast<AAgentsCharacter>(Manager->GetAgent(AgentId));

		/*if (ForwardValue < 0.0f)
		{
			ForwardValue = -1.0f;
		}
		else if (ForwardValue > 0.0f)
		{
			ForwardValue = 1.0f;
		}
		else
		{
			ForwardValue = 0.0f;
		}*/

		RightValue /= 5.0f;

		FVector2D MovementVector(RightValue, ForwardValue < 0.0f ? -0.2f : 1.0f);
		//FVector2D MovementVector(0.0f, ForwardValue);
		FInputActionValue MovementInputActionValue(MovementVector);

		Agent->Move(MovementInputActionValue);
		Agent->Turn(TurnValue);

		
		
	}
	// UE_LOG(LogTemp, Warning, TEXT("---------------------------------"));
}

void UAgentsInteractor::PostInitProperties()
{
	//UE_LOG(LogTemp, Warning, TEXT("PostInitProperties"));

	Super::PostInitProperties();


	//ALearningAgentsManager* AManager = GetOwner<ALearningAgentsManager>();
	//UE_LOG(LogTemp, Warning, TEXT("PostInitProperties"));
	// int a = 1;
}
