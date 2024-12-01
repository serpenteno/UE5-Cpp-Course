// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AlucardAnimInstance.generated.h"

class AAlucardCharacter;
class UCharacterMovementComponent;

enum class ECharacterState : uint8;
enum class EActionState    : uint8;
enum class EDeathPose      : uint8;

UCLASS()
class ALUCARD_API UAlucardAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation()            override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

public:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AAlucardCharacter> AlucardCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	TObjectPtr<UCharacterMovementComponent> AlucardCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsFalling;

	UPROPERTY(BlueprintReadOnly, Category = "Movement|States")
	ECharacterState CharacterState;

	UPROPERTY(BlueprintReadOnly, Category = "Movement|States")
	EActionState ActionState;

	UPROPERTY(BlueprintReadOnly, Category = "Movement|States")
	EDeathPose DeathPose;
};
