// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AlucardCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/AlucardHUD.h"
#include "HUD/AlucardOverlay.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Weapons/WeaponTypes.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "Items/Potions/ResourceTypes.h"

// Sets default values
AAlucardCharacter::AAlucardCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->bUsePawnControlRotation = true;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void AAlucardCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("AlucardCharacter"));

	if (TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(GetController()))
	{
		if (TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(AlucardMappingContext, 0);
		}

		if (TObjectPtr<AAlucardHUD> AlucardHUD = Cast<AAlucardHUD>(PlayerController->GetHUD()))
		{
			if (AlucardOverlay = AlucardHUD->GetAlucardOverlay())
			{
				if (AttributeComponent)
				{
					AlucardOverlay->SetHealthBarPercent(AttributeComponent->GetHealthPercent());
					// AlucardOverlay->SetStaminaBarPercent(AttributeComponent->GetStaminaPercent();
				}
			}
		}
	}
}

// Called every frame
void AAlucardCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AttributeComponent)
	{
		AttributeComponent->RegenStamina(DeltaTime);

		if (AlucardOverlay)
		{
			AlucardOverlay->SetHealthBarPercent(AttributeComponent->GetHealthPercent());
			AlucardOverlay->SetStaminaBarPercent(AttributeComponent->GetStaminaPercent());
		}
	}
}

// Called to bind functionality to input
void AAlucardCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAlucardCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAlucardCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AAlucardCharacter::Jump);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AAlucardCharacter::Interact);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AAlucardCharacter::Attack);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &AAlucardCharacter::Dodge);
	}
}

void AAlucardCharacter::Jump()
{
	if (IsUnoccupied())
	{
		Super::Jump();
	}

}

float AAlucardCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	SetHUDHealth();

	return DamageAmount;
}

void AAlucardCharacter::GetHit_Implementation(const FVector& InImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(InImpactPoint, Hitter);

	if (AttributeComponent && AttributeComponent->GetHealthPercent() > 0.0f)
	{

		ActionState = EActionState::EAS_HitReaction;
	}
}

void AAlucardCharacter::SetOverlappingItem(const TObjectPtr<AItem> Item)
{
	OverlappingItem = Item;
}

void AAlucardCharacter::AddGold(const TObjectPtr<ATreasure> Treasure)
{
	if (AttributeComponent && AlucardOverlay)
	{
		AttributeComponent->AddGold(Treasure->GetGold());
		AlucardOverlay->SetGoldCount(AttributeComponent->GetGold());
	}
}

void AAlucardCharacter::AddSoul(const TObjectPtr<ASoul> Soul)
{
	if (AttributeComponent && AlucardOverlay)
	{
		AttributeComponent->AddSouls(Soul->GetSouls());
		AlucardOverlay->SetSoulsCount(AttributeComponent->GetSouls());
	}
}

void AAlucardCharacter::RestoreResource(const EResourceType ResourceType, const float ResourceAmount)
{
	if (AttributeComponent)
	{
		switch (ResourceType)
		{
		case EResourceType::Health:
			if (AttributeComponent->GetHealthPercent() < 1.0f)
			{
				AttributeComponent->SetHealth(FMath::Clamp(AttributeComponent->GetHealth() + ResourceAmount, 0.0f, AttributeComponent->GetMaxHealth()));
			}
			break;

		case EResourceType::Stamina:
			if (AttributeComponent->GetStaminaPercent() < 1.0f)
			{
				AttributeComponent->SetStamina(FMath::Clamp(AttributeComponent->GetStamina() + ResourceAmount, 0.0f, AttributeComponent->GetMaxStamina()));
			}
			break;

		default:
			break;
		}
	}
}

void AAlucardCharacter::Move(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied || GetCharacterMovement()->IsFalling())
	{
		return;
	}

	const FVector2D DirectionValue = Value.Get<FVector2D>();

	if (GetController())
	{
		// Find out which way is forward and right
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);

		// Rotation matrix for yaw rotations only
		const FRotationMatrix YawRotationMatrix(YawRotation);
		const FVector ControlForwardDirection = YawRotationMatrix.GetUnitAxis(EAxis::X);
		const FVector ControlRightDirection = YawRotationMatrix.GetUnitAxis(EAxis::Y);

		// Matches the character's move direction to the controller's current rotation
		AddMovementInput(ControlForwardDirection, DirectionValue.Y);
		AddMovementInput(ControlRightDirection, DirectionValue.X);
	}
}

void AAlucardCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	if (GetController())
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void AAlucardCharacter::Interact(const FInputActionValue& Value)
{
	if (OverlappingItem)
	{
		TObjectPtr<AWeapon> OverlappingWeapon = Cast<AWeapon>(OverlappingItem);

		if (OverlappingWeapon && ActionState == EActionState::EAS_Unoccupied)
		{
			if (EquippedWeapon)
			{
				EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				EquippedWeapon->PlaceItemOnGround();
				EquippedWeapon->SetItemState(EItemState::EIS_Hovering);
			}

			FName WeaponSocketName;

			switch (OverlappingWeapon->GetWeaponType())
			{
			case EWeaponType::EWT_OneHanded:
				CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
				WeaponSocketName = FName("RightHandSocket_OneHandedWeapon");
				break;

			case EWeaponType::EWT_TwoHanded:
				CharacterState = ECharacterState::ECS_EquippedTwoHandedWeapon;
				WeaponSocketName = FName("RightHandSocket_TwoHandedWeapon");
				break;

			default:
				break;
			}

			OverlappingWeapon->Equip(GetMesh(), WeaponSocketName, this, this);
			OverlappingItem = nullptr;
			EquippedWeapon = OverlappingWeapon;
		}
	}
	else
	{
		if (CanDisarm())
		{
			PlayEquipMontage(FName("Unequip"));
			CharacterState = ECharacterState::ECS_Unequipped;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
		else if (CanArm())
		{
			PlayEquipMontage(FName("Equip"));
			switch (EquippedWeapon->GetWeaponType())
			{
			case EWeaponType::EWT_OneHanded:
				CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
				break;

			case EWeaponType::EWT_TwoHanded:
				CharacterState = ECharacterState::ECS_EquippedTwoHandedWeapon;
				break;

			default:
				break;
			}
			ActionState = EActionState::EAS_EquippingWeapon;
		}
	}
}

void AAlucardCharacter::Attack(const FInputActionValue& Value)
{
	if (AttackMontage && CanAttack())
	{
		PlayRandomMontageSection(AttackMontage);
		ActionState = EActionState::EAS_Attacking;
	}
}

void AAlucardCharacter::Dodge(const FInputActionValue& Value)
{
	if (DodgeMontage && CanDodge())
	{
		PlayRandomMontageSection(DodgeMontage);
		if (AttributeComponent)
		{
			AttributeComponent->UseStamina(AttributeComponent->GetDodgeCost());
		}
		ActionState = EActionState::EAS_Dodging;
	}
}

void AAlucardCharacter::Die_Implementation()
{
	Super::Die_Implementation();

	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
}

void AAlucardCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool AAlucardCharacter::CanAttack() const
{
	return CharacterState != ECharacterState::ECS_Unequipped && ActionState == EActionState::EAS_Unoccupied;
}

void AAlucardCharacter::PlayEquipMontage(const FName& InSectionName) const
{
	const TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();
	
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(InSectionName, EquipMontage);
	}
}

bool AAlucardCharacter::CanArm() const
{
	return CharacterState == ECharacterState::ECS_Unequipped && ActionState == EActionState::EAS_Unoccupied && EquippedWeapon;
}

bool AAlucardCharacter::CanDisarm() const
{
	return CharacterState != ECharacterState::ECS_Unequipped && ActionState == EActionState::EAS_Unoccupied;
}

bool AAlucardCharacter::CanDodge() const
{
	if (AttributeComponent)
	{
		return ActionState == EActionState::EAS_Unoccupied && AttributeComponent->HasEnoughStamina();
	}
	else
	{
		return ActionState == EActionState::EAS_Unoccupied;
	}
}

void AAlucardCharacter::EquipEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AAlucardCharacter::AttachWeaponToHand()
{
	if (EquippedWeapon)
	{
		switch (EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_OneHanded:
			EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket_OneHandedWeapon"));
			break;

		case EWeaponType::EWT_TwoHanded:
			EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket_TwoHandedWeapon"));
			break;

		default:
			break;
		}
	}
}

void AAlucardCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void AAlucardCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AAlucardCharacter::DodgeEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AAlucardCharacter::SetHUDHealth() const
{
	if (AlucardOverlay && AttributeComponent)
	{
		AlucardOverlay->SetHealthBarPercent(AttributeComponent->GetHealthPercent());
	}
}

bool AAlucardCharacter::IsUnoccupied() const
{
	return ActionState == EActionState::EAS_Unoccupied;
}

