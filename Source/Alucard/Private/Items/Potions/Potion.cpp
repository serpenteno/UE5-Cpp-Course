// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Potions/Potion.h"

void APotion::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IPickUpInterface* PickUpInterface = Cast<IPickUpInterface>(OtherActor))
	{
		PickUpInterface->RestoreResource(ResourceType, ResourceAmount);
		Destroy();
	}
}
