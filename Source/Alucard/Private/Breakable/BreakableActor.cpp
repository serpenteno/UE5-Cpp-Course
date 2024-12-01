// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Treasure.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ABreakableActor::ABreakableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GeometryCollection->bNotifyBreaks = true;

	TreasureSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("TreasureTransform"));
	TreasureSpawn->SetupAttachment(GetRootComponent());

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(GetRootComponent());
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Block);
}

// Called when the game starts or when spawned
void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
	GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakableActor::OnChaosBreakEvent);
}

// Called every frame
void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& InImpactPoint, AActor* Hitter)
{
	if (bIsBroken)
	{
		return;
	}

	bIsBroken = true;

	if (GetWorld())
	{
		if (TreasureClasses.Num() > 0 && bDropsTreasure)
		{
			const uint8 Selection = FMath::RandRange(0, TreasureClasses.Num() - 1);
			GetWorld()->SpawnActor<ATreasure>(TreasureClasses[Selection], TreasureSpawn->GetComponentLocation(), TreasureSpawn->GetComponentRotation());
		}
	}
}

void ABreakableActor::OnChaosBreakEvent(const FChaosBreakEvent& BreakEvent)
{
	SetLifeSpan(BrokenPiecesLifeSpan);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Ignore);
}

