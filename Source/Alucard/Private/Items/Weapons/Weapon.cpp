// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/AlucardCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnWeaponBoxBeginOverlap);
}

void AWeapon::Equip(const TObjectPtr<USceneComponent> InParent, const FName& InSocketName, const TObjectPtr<AActor> InOwner, const TObjectPtr<APawn> InInstigator, const bool bPlayEquipSound)
{
	SetOwner(InOwner);
	SetInstigator(InInstigator);
	AttachMeshToSocket(InParent, InSocketName);
	ItemState = EItemState::EIS_Equipped;
	DisableSphereCollision();
	if (bPlayEquipSound)
	{
		SpawnPickUpSound();
	}
	DeactivateEmbers();
}

void AWeapon::DeactivateEmbers()
{
	if (ItemEffect)
	{
		ItemEffect->Deactivate();
	}
}

void AWeapon::DisableSphereCollision()
{
	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeapon::AttachMeshToSocket(const TObjectPtr<USceneComponent> InParent, const FName& InSocketName)
{
	FAttachmentTransformRules AttachmentTranformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
	AttachToComponent(InParent, AttachmentTranformRules, InSocketName);
}

bool AWeapon::IsActorSameType(const TObjectPtr<AActor> OtherActor)
{
	return (GetOwner()->ActorHasTag(FName("Enemy")) && OtherActor->ActorHasTag(FName("Enemy"))) || (GetOwner()->ActorHasTag(FName("AlucardCharacter")) && OtherActor->ActorHasTag(FName("AlucardCharacter")));
}

void AWeapon::ExecuteGetHit(FHitResult& BoxHit)
{
	if (IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor()))
	{
		UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigatorController(), this, UDamageType::StaticClass());
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
	}
}

void AWeapon::OnWeaponBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsActorSameType(OtherActor))
	{
		return;
	}

	FHitResult BoxHit;
	BoxTrace(BoxHit);

	if (BoxHit.GetActor())
	{
		if (IsActorSameType(BoxHit.GetActor()))
		{
			return;
		}

		ExecuteGetHit(BoxHit);
	}
}

void AWeapon::BoxTrace(FHitResult& OutBoxHit)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();
	TArray<TObjectPtr<AActor>> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());

	for (const TObjectPtr<AActor> Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(this, Start, End, BoxTraceExtent, BoxTraceStart->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, OutBoxHit, true);
	IgnoreActors.AddUnique(OutBoxHit.GetActor());
}
