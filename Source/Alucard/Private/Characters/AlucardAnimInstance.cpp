// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AlucardAnimInstance.h"
#include "Characters/AlucardCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UAlucardAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AlucardCharacter = Cast<AAlucardCharacter>(TryGetPawnOwner()))
	{
		AlucardCharacterMovement = AlucardCharacter->GetCharacterMovement();
	}
}

void UAlucardAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (AlucardCharacterMovement)
	{
		GroundSpeed    = UKismetMathLibrary::VSizeXY(AlucardCharacterMovement->Velocity);
		bIsFalling     = AlucardCharacterMovement->IsFalling();
		CharacterState = AlucardCharacter->GetCharacterState();
		ActionState    = AlucardCharacter->GetActionState();
		DeathPose      = AlucardCharacter->GetDeathPose();
	}
}
