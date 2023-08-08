// Fill out your copyright notice in the Description page of Project Settings.


#include "AgentsAgentWidgetComponent.h"

#include "Components/TextBlock.h"

void UAgentsAgentWidgetComponent::SetText(FString Text)
{
	if (UTextBlock* TextBlock = Cast<UTextBlock>(GetUserWidgetObject()->GetWidgetFromName("OverheadText")))
	{
		TextBlock->SetText(FText::FromString(Text));
	}
}
