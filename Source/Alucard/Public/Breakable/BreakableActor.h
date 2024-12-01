// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;
class ATreasure;
class UCapsuleComponent;

UCLASS()
class ALUCARD_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABreakableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual void GetHit_Implementation(const FVector& InImpactPoint, AActor* Hitter) override;

	UFUNCTION()
	virtual void OnChaosBreakEvent(const FChaosBreakEvent& BreakEvent);

	UPROPERTY(EditAnywhere)
	float BrokenPiecesLifeSpan = 3.0f;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UGeometryCollectionComponent> GeometryCollection;

	UPROPERTY(EditAnywhere, Category = "Treasure")
	TArray<TSubclassOf<ATreasure>> TreasureClasses;

	UPROPERTY(EditAnywhere, Category = "Treasure")
	TObjectPtr<USceneComponent> TreasureSpawn;

	UPROPERTY(EditAnywhere, Category = "Treasure")
	bool bDropsTreasure = true;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCapsuleComponent> Capsule;

	bool bIsBroken = false;

};
