// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Soul.generated.h"

class UNiagaraSystem;

UCLASS()
class ALUCARD_API ASoul : public AItem
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	float DesiredZ;
	
	UPROPERTY(EditAnywhere)
	float DriftRate = 15.0f;

	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	uint32 Souls;

public:
	FORCEINLINE uint32 GetSouls() const { return Souls; }

	FORCEINLINE void SetSouls(const uint32 NumberOfSouls) { Souls = NumberOfSouls; }
};
