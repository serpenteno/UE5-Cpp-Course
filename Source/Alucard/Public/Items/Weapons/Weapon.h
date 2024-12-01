// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

enum class EWeaponType : uint8;
class UBoxComponent;

UCLASS()
class ALUCARD_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();

protected:
	virtual void BeginPlay() override;

public:
	void Equip(const TObjectPtr<USceneComponent> InParent, const FName& InSocketName, const TObjectPtr<AActor> InOwner, const TObjectPtr<APawn> Instigator, const bool bPlayEquipSound = true);
	void DeactivateEmbers();
	void DisableSphereCollision();
	void AttachMeshToSocket(const TObjectPtr<USceneComponent> InParent, const FName& InSocketName);

	UPROPERTY(VisibleInstanceOnly)
	TArray<TObjectPtr<AActor>> IgnoreActors;

protected:
	bool IsActorSameType(const TObjectPtr<AActor> OtherActor);
	void ExecuteGetHit(FHitResult& BoxHit);

	UFUNCTION()
	virtual void OnWeaponBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	void BoxTrace(FHitResult& OutBoxHit);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxTraceExtent = FVector(5.0f);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowBoxDebug = false;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TObjectPtr<UBoxComponent> WeaponBox;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 20.0f;;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> BoxTraceStart;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> BoxTraceEnd;

public:
	FORCEINLINE EWeaponType GetWeaponType() const				{ return WeaponType; }
	FORCEINLINE TObjectPtr<UBoxComponent> GetWeaponBox() const	{ return WeaponBox; }
};
