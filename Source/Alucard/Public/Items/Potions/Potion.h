// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Potion.generated.h"

enum class EResourceType : uint8;

UCLASS()
class ALUCARD_API APotion : public AItem
{
	GENERATED_BODY()
	
protected:
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	UPROPERTY(EditAnywhere, Category = "Resources")
	float ResourceAmount = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Resources")
	EResourceType ResourceType;

};
