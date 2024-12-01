// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AlucardOverlay.generated.h"

class UProgressBar;
class UTextBlock;

/**
 *
 */
UCLASS()
class ALUCARD_API UAlucardOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealthBarPercent(const float InPercent);
	void SetStaminaBarPercent(const float InPercent);
	void SetGoldCount(const int32 InGoldCount);
	void SetSoulsCount(const int32 InSoulsCount);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GoldCountText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SoulsCountText;

};
