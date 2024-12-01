// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
// #include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Items/Weapons/Weapon.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attribute"));

	HealthBarComponent = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);

	if (TObjectPtr<ABaseCharacter> Hitter = Cast<ABaseCharacter>(EventInstigator->GetPawn()))
	{
		Hitter->CombatTarget = this;
	}

	return DamageAmount;
}

void ABaseCharacter::GetHit_Implementation(const FVector& InImpactPoint, AActor* Hitter)
{
	if (IsAlive() && Hitter)
	{
		DirectionalHitReact(Hitter->GetActorLocation());
	}
	else
	{
		Die();
	}

	PlayHitSound(InImpactPoint);
	SpawnHitParticles(InImpactPoint);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	StopAttackMontage();
}

void ABaseCharacter::HandleDamage(const float InDamageAmount)
{
	if (AttributeComponent)
	{
		AttributeComponent->ReceiveDamage(InDamageAmount);
	}
}

void ABaseCharacter::DirectionalHitReact(const FVector& InImpactPoint)
{
	const FVector& Forward = GetActorForwardVector();
	const FVector ToHit = (InImpactPoint - GetActorLocation()).GetSafeNormal2D();
	// Forward * ToHit = |Forward||ToHit| * cos(theta)
	// |Forward| = 1, |ToHit| = 1, so Forward * ToHit = cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	// Take the inverse cosine (arc-cosine) of cos(theta) to get theta
	double Theta = FMath::Acos(CosTheta);
	// convert from radians to degrees
	Theta = FMath::RadiansToDegrees(Theta);
	// If CrossProduct points down , Angle should be negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);

	if (CrossProduct.Z < 0.0f)
	{
		Theta *= -1.0f;
	}

	FName Section("FromBack");
	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("FromRight");
	}

	/*
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 100.0f, 5.0f, FLinearColor::Blue, 5.0f);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Green, FString::Printf(TEXT("Angle: %f"), Theta));
	}

	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 50.0f, 5.0f, FLinearColor::Red, 5.0f);
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 50.0f, 5.0f, FLinearColor::Green, 5.0f);
	*/

	PlayMontage(HitReactMontage, Section);
}

void ABaseCharacter::PlayHitSound(const FVector& InImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, InImpactPoint);
	}
}

void ABaseCharacter::SpawnHitParticles(const FVector& InImpactPoint)
{
	if (HitParticle && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, InImpactPoint);
	}
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::DisableMesh()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

bool ABaseCharacter::IsAlive() const
{
	return AttributeComponent && AttributeComponent->IsAlive();
}

void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::Die_Implementation()
{
	PlayRandomMontageSection(DeathMontage);
	Tags.Add(FName("Dead"));
}

void ABaseCharacter::PlayMontage(const TObjectPtr<UAnimMontage> AnimMontage, const FName& InSectionName) const
{
	const TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AnimMontage)
	{
		AnimInstance->Montage_Play(AnimMontage);
		AnimInstance->Montage_JumpToSection(InSectionName, AnimMontage);
	}
}

int32 ABaseCharacter::PlayRandomMontageSection(const TObjectPtr<UAnimMontage> AnimMontage) const
{
	if (AnimMontage)
	{
		const int32 Selection = FMath::RandRange(0, AnimMontage->GetNumSections() - 1);
		PlayMontage(AnimMontage, AnimMontage->GetSectionName(Selection));
		return Selection;
	}

	return 0;
}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}

void ABaseCharacter::StopAttackMontage()
{
	if (TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Stop(0.25f, AttackMontage);
	}
}

FVector ABaseCharacter::GetTranslationWarpTarget() const
{
	if (!CombatTarget)
	{
		return FVector();
	}

	const FVector& Location = GetActorLocation();
	const FVector& CombatTargetLocation = CombatTarget->GetActorLocation();
	
	FVector TargetToThis = (Location - CombatTargetLocation).GetSafeNormal2D() * WarpTargetDistance;
	
	return CombatTargetLocation + TargetToThis;
}

FVector ABaseCharacter::GetRotationWarpTarget() const
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

