// Fill out your copyright notice in the Description page of Project Settings.


#include "AgentsMenu.h"

#include "Agents/AgentsGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"


bool UAgentsMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (TrainButton)
	{
		TrainButton->OnClicked.AddDynamic(this, &UAgentsMenu::TrainButtonClicked);
	}

	if (ContinueTrainButton)
	{
		ContinueTrainButton->OnClicked.AddDynamic(this, &UAgentsMenu::ContinueTrainingButtonClicked);
	}

	if (PlayButton)
	{
		PlayButton->OnClicked.AddDynamic(this, &UAgentsMenu::PlayButtonClicked);
	}

	if (PlayPretrainedButton)
	{
		PlayPretrainedButton->OnClicked.AddDynamic(this, &UAgentsMenu::PlayPretrainedButtonClicked);
	}

	return true;
}

void UAgentsMenu::SetupMenu()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}
}

void UAgentsMenu::PlayButtonClicked()
{
	UWorld* World = GetWorld();
	if (World)
	{
		AAgentsGameMode* GameMode = Cast<AAgentsGameMode>(UGameplayStatics::GetGameMode(World));
		if (GameMode)
		{
			GameMode->StartPlaying();
		}
	}

	MenuTearDown();
}

void UAgentsMenu::PlayPretrainedButtonClicked()
{
	UWorld* World = GetWorld();
	if (World)
	{
		AAgentsGameMode* GameMode = Cast<AAgentsGameMode>(UGameplayStatics::GetGameMode(World));
		if (GameMode)
		{
			GameMode->StartPlayingPretrained();
		}
	}

	MenuTearDown();
}

void UAgentsMenu::TrainButtonClicked()
{
	UWorld* World = GetWorld();
	if (World)
	{
		AAgentsGameMode* GameMode = Cast<AAgentsGameMode>(UGameplayStatics::GetGameMode(World));
		if (GameMode)
		{
			GameMode->StartTraining();
		}
	}

	MenuTearDown();
}

void UAgentsMenu::ContinueTrainingButtonClicked()
{
	UWorld* World = GetWorld();
	if (World)
	{
		AAgentsGameMode* GameMode = Cast<AAgentsGameMode>(UGameplayStatics::GetGameMode(World));
		if (GameMode)
		{
			GameMode->ContinueTraining();
		}
	}

	MenuTearDown();
}

void UAgentsMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}


