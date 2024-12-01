// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/AlucardHUD.h"
#include "HUD/AlucardOverlay.h"

void AAlucardHUD::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	if (GetWorld() && GetWorld()->GetFirstPlayerController() && AlucardOverlayClass)
	{
		AlucardOverlay = CreateWidget<UAlucardOverlay>(GetWorld()->GetFirstPlayerController(), AlucardOverlayClass);
		AlucardOverlay->AddToViewport();
	}
}
