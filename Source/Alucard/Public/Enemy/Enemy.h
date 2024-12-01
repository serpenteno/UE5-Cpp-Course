// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class AAIController;
class UPawnSensingComponent;
class ASoul;

UCLASS()
class ALUCARD_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	/** <ABaseCharacter> */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	/** </ABaseCharacter> */

	/** <IHitInterface> */
	virtual void GetHit_Implementation(const FVector& InImpactPoint, AActor* Hitter) override;
	/** </IHitInterface> */

protected:
	/** <ABaseCharacter> */
	virtual void Die_Implementation() override;
	virtual bool CanAttack() const override;
	virtual void HandleDamage(const float InDamageAmount) override;
	virtual void AttackEnd() override;
	/** </ABaseCharacter> */

	void Attack();
	void SpawnSoul() const;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

private:
	/** AI Behavior */
	void InitializeEnemy();
	void CheckPatrolTarget();
	void CheckCombatTarget();
	void OnPatrolTimerFinished();
	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius() const;
	bool IsOutsideAttackRadius() const;
	bool IsInsideAttackRadius()	const;
	bool IsChasing() const;
	bool IsAttacking() const;
	bool IsDead() const;
	bool IsEngaged() const;
	void ClearPatrolTimer();
	void StartAttackTimer();
	void ClearAttackTimer();
	bool IsInTargetRange(const TObjectPtr<AActor> InTarget, const float InRadius) const;
	void MoveToTarget(const TObjectPtr<AActor> InTarget);
	TObjectPtr<AActor> ChoosePatrolTarget() const;
	void SpawnDefaultWeapon();

	UFUNCTION()
	void OnSeePawn(APawn* InSeenPawn); // Callback for OnPawnSeen in UPawnSensingComponent

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPawnSensingComponent> PawnSensingComponent;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere)
	float CombatRadius = 500.0f;

	UPROPERTY(EditAnywhere)
	float AttackRadius = 150.0f;

	UPROPERTY()
	TObjectPtr<AAIController> EnemyController;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TObjectPtr<AActor> PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<TObjectPtr<AActor>> PatrolTargets; // Patrol targets to move enemy to

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolRadius = 200.0f;

	UPROPERTY(EditAnywhere)
	float CorpseLifeSpan = 0.0f;

	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float MinPatrolWaitTime = 5.0f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float MaxPatrolWaitTime = 10.0f;

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<ASoul> SoulClass;

	FTimerHandle SpawnSoulTimer;

};
