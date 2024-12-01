// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AlucardHUD.generated.h"

class UAlucardOverlay;

/**
 * 
 */
UCLASS()
class ALUCARD_API AAlucardHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	virtual void PreInitializeComponents() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Alucard")
	TSubclassOf<UAlucardOverlay> AlucardOverlayClass;
	
	UPROPERTY()
	TObjectPtr<UAlucardOverlay> AlucardOverlay;

public:
	FORCEINLINE TObjectPtr<UAlucardOverlay> GetAlucardOverlay() const { return AlucardOverlay; }

};
