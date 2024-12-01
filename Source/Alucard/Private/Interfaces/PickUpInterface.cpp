// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/PickUpInterface.h"

// Add default functionality here for any IPickUpInterface functions that are not pure virtual.

void IPickUpInterface::SetOverlappingItem(const TObjectPtr<AItem> Item)
{

}

void IPickUpInterface::AddGold(const TObjectPtr<ATreasure> Treasure)
{

}

void IPickUpInterface::AddSoul(const TObjectPtr<ASoul> Soul)
{

}

void IPickUpInterface::RestoreResource(const EResourceType ResourceType, const float ResourceAmount)
{

}
