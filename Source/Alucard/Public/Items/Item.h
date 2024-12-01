// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/PickUpInterface.h"
#include "Item.generated.h"

class USphereComponent;
class UNiagaraComponent;
class UNiagaraSystem;

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Equipped
};

UCLASS()
class ALUCARD_API AItem : public AActor, public IPickUpInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable)
	void PlaceItemOnGround();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HoveringHeight = 85.0f;

protected:
	virtual void SpawnPickUpSystem() const;
	virtual void SpawnPickUpSound() const;
	
	UFUNCTION(BlueprintPure)
	float TransformSine() const;

	UFUNCTION(BlueprintPure)
	float TransformCosine() const;

	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float Amplitude = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float TimeConstant = 5.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator HoveringRotation;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	EItemState ItemState = EItemState::EIS_Hovering;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraComponent> ItemEffect;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> PickUpEffect;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> PickUpSound;

private:
	float CalculateHoveringHeight() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime = 0.0f;

public:
	FORCEINLINE EItemState GetItemState() const							{ return ItemState; }
	FORCEINLINE TObjectPtr<UStaticMeshComponent> GetItemMesh() const	{ return ItemMesh; }
	FORCEINLINE void SetItemState(EItemState InItemState)				{ ItemState = InItemState; }

};
