// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Soul.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	bUseControllerRotationYaw = false;

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensingComponent->SetPeripheralVisionAngle(45.0f);
	PawnSensingComponent->SightRadius = 4000.0f;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (PawnSensingComponent)
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemy::OnSeePawn);
	}

	InitializeEnemy();
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead())
	{
		return;
	}

	HealthBarComponent->FacePlayerCamera();

	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else 
	{
		CheckPatrolTarget();
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	ChaseTarget();

	return Damage;
}

void AEnemy::GetHit_Implementation(const FVector& InImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(InImpactPoint, Hitter);

	if (!IsDead())
	{
		ShowHealthBar();
	}

	ClearPatrolTimer();
	ClearAttackTimer();
	if (IsInsideAttackRadius())
	{
		if (!IsDead())
		{
			StartAttackTimer();
		}
	}

}

void AEnemy::Die_Implementation()
{
	Super::Die_Implementation();

	EnemyState = EEnemyState::EES_Dead;
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

	if (TObjectPtr<ABaseCharacter> BaseCharacter = Cast<ABaseCharacter>(CombatTarget))
	{
		if (BaseCharacter->CombatTarget == this)
		{
			BaseCharacter->CombatTarget = nullptr;
		}
	}

	if (EquippedWeapon)
	{
		EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		EquippedWeapon->PlaceItemOnGround();
		EquippedWeapon->SetItemState(EItemState::EIS_Hovering);
	}
	
	const int32 Selection = PlayRandomMontageSection(DeathMontage);
	if (Selection < static_cast<int32>(EDeathPose::EDP_MAX))
	{
		DeathPose = static_cast<EDeathPose>(Selection);
	}

	ClearAttackTimer();
	HideHealthBar();
	DisableCapsule();
	DisableMesh();
	SetLifeSpan(CorpseLifeSpan);
	GetCharacterMovement()->bOrientRotationToMovement = false;

	GetWorldTimerManager().SetTimer(SpawnSoulTimer, this, &AEnemy::SpawnSoul, 1.0f);
}

bool AEnemy::CanAttack() const
{
	const bool bCanAttack = IsInsideAttackRadius() && !IsAttacking() && !IsEngaged() && !IsDead();
	return bCanAttack;
}

void AEnemy::HandleDamage(const float InDamageAmount)
{
	Super::HandleDamage(InDamageAmount);

	if (HealthBarComponent && AttributeComponent)
	{
		HealthBarComponent->SetHealthPercent(AttributeComponent->GetHealthPercent());
	}
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
		return;
	}
	EnemyState = EEnemyState::EES_Engaged;
	PlayRandomMontageSection(AttackMontage);
}

void AEnemy::SpawnSoul() const
{
	if (GetWorld() && SoulClass && AttributeComponent)
	{
		ASoul* SpawnedSoul = GetWorld()->SpawnActor<ASoul>(SoulClass, GetActorLocation(), GetActorRotation());
		if (SpawnedSoul)
		{
			SpawnedSoul->SetSouls(AttributeComponent->GetSouls());
		}
	}
}

void AEnemy::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	Tags.Add(FName("Enemy"));


	if (HealthBarComponent && AttributeComponent)
	{
		HealthBarComponent->SetHealthPercent(AttributeComponent->GetHealthPercent());
		HideHealthBar();
	}

	MoveToTarget(PatrolTarget);
	SpawnDefaultWeapon();
}

void AEnemy::CheckPatrolTarget()
{
	if (IsInTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float PatrolWaitTime = FMath::RandRange(MinPatrolWaitTime, MaxPatrolWaitTime);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::OnPatrolTimerFinished, PatrolWaitTime);
	}
}

void AEnemy::CheckCombatTarget()
{
	if (IsOutsideCombatRadius())
	{
		ClearAttackTimer();
		LoseInterest();
		if (!IsEngaged())
		{
			StartPatrolling();
		}
	}
	else if (IsOutsideAttackRadius() && !IsChasing())
	{
		ClearAttackTimer();
		if (!IsEngaged())
		{
			ChaseTarget();
		}
	}
	else if (CanAttack())
	{
		StartAttackTimer();
	}
}

void AEnemy::OnPatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::HideHealthBar()
{
	if (HealthBarComponent)
	{
		HealthBarComponent->SetVisibility(false);
	}
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarComponent)
	{
		HealthBarComponent->SetVisibility(true);
	}
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	MoveToTarget(PatrolTarget);
}

void AEnemy::ChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	MoveToTarget(CombatTarget);
}

bool AEnemy::IsOutsideCombatRadius() const
{
	return !IsInTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius() const
{
	return !IsInTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInsideAttackRadius() const
{
	return IsInTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsChasing() const
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking() const
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsDead() const
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsEngaged() const
{
	return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

bool AEnemy::IsInTargetRange(const TObjectPtr<AActor> InTarget, const float InRadius) const
{
	if (!InTarget)
	{
		return false;
	}
	const float DistanceToTarget = (InTarget->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= InRadius;
}

void AEnemy::MoveToTarget(const TObjectPtr<AActor> Target)
{
	if (!EnemyController || !Target || EnemyState == EEnemyState::EES_Dead)
	{
		return;
	}

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(50.0f);
	EnemyController->MoveTo(MoveRequest);
}

TObjectPtr<AActor> AEnemy::ChoosePatrolTarget() const
{
	TArray<TObjectPtr<AActor>> ValidTargets;
	for (const TObjectPtr<AActor> Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	if (ValidTargets.Num() > 0)
	{
		const uint8 TargetSelection = FMath::RandRange(0, ValidTargets.Num() - 1);
		return ValidTargets[TargetSelection];
	}

	return nullptr;
}

void AEnemy::SpawnDefaultWeapon()
{
	if (GetWorld() && WeaponClass)
	{
		TObjectPtr<AWeapon> Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
		Weapon->Equip(GetMesh(), FName("WeaponSocket"), this, this, false);
		EquippedWeapon = Weapon;
	}
}

void AEnemy::OnSeePawn(APawn* SeenPawn)
{
	const bool bShouldChaseTarget = EnemyState != EEnemyState::EES_Dead && EnemyState != EEnemyState::EES_Chasing && EnemyState < EEnemyState::EES_Attacking && SeenPawn->ActorHasTag(FName("AlucardCharacter")) && !SeenPawn->ActorHasTag(FName("Dead"));
	
	if (bShouldChaseTarget)
	{
		CombatTarget = SeenPawn;
		ClearPatrolTimer();
		ChaseTarget();
	}
}

