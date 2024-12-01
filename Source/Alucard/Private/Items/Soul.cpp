// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Soul.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void ASoul::BeginPlay()
{
	Super::BeginPlay();

	const FVector Start = GetActorLocation();
	const FVector End = Start - FVector(0.0f, 0.0f, 2000.0f);
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	FHitResult HitResult;

	UKismetSystemLibrary::LineTraceSingleForObjects(this, Start, End, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
	DesiredZ = HitResult.ImpactPoint.Z + 15.0f;
}

void ASoul::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float LocationZ = GetActorLocation().Z;
	if (LocationZ > DesiredZ)
	{
		AddActorWorldOffset(FVector(0.0f, 0.0f, DriftRate * DeltaTime));
	}
}

void ASoul::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IPickUpInterface* PickUpInterface = Cast<IPickUpInterface>(OtherActor))
	{
		PickUpInterface->AddSoul(this);
		SpawnPickUpSystem();
		SpawnPickUpSound();
		Destroy();
	}
}
