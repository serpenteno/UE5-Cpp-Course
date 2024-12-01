// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UHealthBarComponent;

enum class EDeathPose : uint8;

UCLASS(Abstract)
class ALUCARD_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	/** <AActor> */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	/** </AActor> */

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<AActor> CombatTarget = nullptr;

protected:
	/** <IHitInterface> */
	virtual void GetHit_Implementation(const FVector& InImpactPoint, AActor* Hitter) override;
	/** </IHitInterface> */
	
	/** Combat */
	virtual bool CanAttack() const PURE_VIRTUAL(ABaseCharacter::CanAttack, return false;);
	virtual bool CanDodge()  const PURE_VIRTUAL(ABaseCharacter::CanDodge, return false;);
	virtual void HandleDamage(const float InDamageAmount);
	void DirectionalHitReact(const FVector & InImpactPoint);
	void PlayHitSound(const FVector& InImpactPoint);
	void SpawnHitParticles(const FVector& InImpactPoint);
	void DisableCapsule();
	void DisableMesh();
	bool IsAlive() const;
	void DisableMeshCollision();

	UFUNCTION(BlueprintNativeEvent)
	void Die();
	
	/** Montage */
	void PlayMontage(const TObjectPtr<UAnimMontage> AnimMontage, const FName& InSectionName) const;
	int32 PlayRandomMontageSection(const TObjectPtr<UAnimMontage> AnimMontage) const;
	void StopAttackMontage();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget() const;
	
	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget() const;

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd() PURE_VIRTUAL(ABaseCharacter::AttackEnd);

	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd() PURE_VIRTUAL(ABaseCharacter::DodgeEnd);

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type InCollisionEnabled);

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<AWeapon> EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> DodgeMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float WarpTargetDistance = 75.0f;

	UPROPERTY(EditAnywhere)
	float WalkSpeed = 150.0f;

	UPROPERTY(EditAnywhere)
	float RunSpeed = 300.0f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeComponent> AttributeComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthBarComponent> HealthBarComponent;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	EDeathPose DeathPose;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, Category = "VisualEffects")
	TObjectPtr<UParticleSystem> HitParticle;

public:
	FORCEINLINE EDeathPose GetDeathPose() const { return DeathPose; }

};
