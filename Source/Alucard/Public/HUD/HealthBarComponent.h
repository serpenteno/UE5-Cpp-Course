// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

class UHealthBarWidget;

/**
 * 
 */
UCLASS()
class ALUCARD_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	UHealthBarComponent();

public:
	void SetHealthPercent(const float Percent);
	void FacePlayerCamera();

protected:
	TObjectPtr<UHealthBarWidget> HealthBarWidget;
};
