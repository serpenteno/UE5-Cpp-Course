 // Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarComponent.h"
#include "HUD/HealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "Kismet/KismetMathLibrary.h"

UHealthBarComponent::UHealthBarComponent()
{
	SetGenerateOverlapEvents(false);
}

void UHealthBarComponent::SetHealthPercent(const float Percent)
{
	if (!HealthBarWidget)
	{
		HealthBarWidget = Cast<UHealthBarWidget>(GetUserWidgetObject());
	}

	if (HealthBarWidget && HealthBarWidget->HealthBar)
	{
		HealthBarWidget->HealthBar->SetPercent(Percent);
	}
}

void UHealthBarComponent::FacePlayerCamera()
{
	if (GetWorld())
	{
		if (TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController())
		{
			FVector CameraLocation;
			FRotator CameraRotation;
			PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

			FRotator WidgetRotation = UKismetMathLibrary::FindLookAtRotation(GetComponentLocation(), CameraLocation);
			SetWorldRotation(WidgetRotation);
		}
	}
}
