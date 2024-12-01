// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALUCARD_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttributeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void ReceiveDamage(const float InDamage);
	void UseStamina(const float StaminaCost);
	void RegenStamina(float DeltaTime);
	void AddGold(const uint32 GoldAmount);
	void AddSouls(const uint32 NumberOfSouls);

private:
	// Current health
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health = 100.0f;
		
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth = 100.0f;

	// Current stamina
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina = 100.0f;
		
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxStamina = 100.0f;
		
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float DodgeCost = 20.0f;
		
	// Stamina regeneration rate described in units per second
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate = 8.0f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	uint32 Gold = 0;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	uint32 Souls = 0;

public:
	FORCEINLINE float GetHealth() const			{ return Health; }
	FORCEINLINE float GetMaxHealth() const		{ return MaxHealth; }
	FORCEINLINE float GetHealthPercent() const  { return Health / MaxHealth; }
	FORCEINLINE float GetStamina() const        { return Stamina; }
	FORCEINLINE float GetMaxStamina() const     { return MaxStamina; }
	FORCEINLINE float GetStaminaPercent() const { return Stamina / MaxHealth; }
	FORCEINLINE float GetDodgeCost() const      { return DodgeCost; }
	FORCEINLINE bool IsAlive() const			{ return Health > 0.0f; }
	FORCEINLINE bool HasEnoughStamina() const	{ return Stamina >= DodgeCost; }
	FORCEINLINE uint32 GetGold() const          { return Gold; }
	FORCEINLINE uint32 GetSouls() const         { return Souls; }

	FORCEINLINE void SetHealth(const float InHealth)         { Health = InHealth; }
	FORCEINLINE void SetMaxHealth(const float InMaxHealth)   { MaxHealth = InMaxHealth; }
	FORCEINLINE void SetStamina(const float InStamina)       { Health = InStamina; }
	FORCEINLINE void SetMaxStamina(const float InMaxStamina) { MaxStamina = InMaxStamina; }
};
