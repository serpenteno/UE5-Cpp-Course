// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Treasure.generated.h"

/**
 * 
 */
UCLASS()
class ALUCARD_API ATreasure : public AItem
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ATreasure();

protected:
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere, Category = "Treasure Properties")
	uint32 Gold;

public:
	FORCEINLINE uint32 GetGold() const { return Gold; }

};
