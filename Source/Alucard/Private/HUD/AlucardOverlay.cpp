// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/AlucardOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UAlucardOverlay::SetHealthBarPercent(const float InPercent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(InPercent);
	}
}

void UAlucardOverlay::SetStaminaBarPercent(const float InPercent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(InPercent);
	}
}

void UAlucardOverlay::SetGoldCount(const int32 InGoldCount)
{
	if (GoldCountText)
	{
		const FText Text = FText::FromString(FString::Printf(TEXT("%d"), InGoldCount));
		GoldCountText->SetText(Text);
	}
}

void UAlucardOverlay::SetSoulsCount(const int32 InSoulsCount)
{
	if (SoulsCountText)
	{
		const FText Text = FText::FromString(FString::Printf(TEXT("%d"), InSoulsCount));
		SoulsCountText->SetText(Text);
	}
}
