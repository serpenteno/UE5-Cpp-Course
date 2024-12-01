// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SetRootComponent(Sphere);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(Sphere);
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ItemEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Embers"));
	ItemEffect->SetupAttachment(ItemMesh);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereBeginOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	HoveringRotation = GetActorRotation();
	PlaceItemOnGround();
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.0f, 0.0f, TransformSine()));
	}
}

void AItem::PlaceItemOnGround()
{
	float ItemHeight = CalculateHoveringHeight();
	FVector ItemLocation = FVector(GetActorLocation().X, GetActorLocation().Y, ItemHeight);
	SetActorTransform(FTransform(HoveringRotation, ItemLocation, GetActorScale()));

	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	if (ItemEffect)
	{
		ItemEffect->Activate();
	}
}

void AItem::SpawnPickUpSystem() const
{
	if (PickUpEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PickUpEffect, GetActorLocation());
	}
}

void AItem::SpawnPickUpSound() const
{
	if (PickUpSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, PickUpSound, GetActorLocation());
	}
}

float AItem::TransformSine() const
{
	return Amplitude * FMath::Sin(TimeConstant * RunningTime);
}

float AItem::TransformCosine() const
{
	return Amplitude * FMath::Cos(TimeConstant * RunningTime);
}

void AItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (IPickUpInterface* PickUpInterface = Cast<IPickUpInterface>(OtherActor))
		{
			PickUpInterface->SetOverlappingItem(this);
		}
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		if (IPickUpInterface* PickUpInterface = Cast<IPickUpInterface>(OtherActor))
		{
			PickUpInterface->SetOverlappingItem(nullptr);
		}
	}
}

float AItem::CalculateHoveringHeight() const
{
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation - FVector(0.0f, 0.0f, 1000.0f);
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility, CollisionParams))
	{
		return HitResult.Location.Z + HoveringHeight;
	}
	else
	{
		return StartLocation.Z;
	}
}

