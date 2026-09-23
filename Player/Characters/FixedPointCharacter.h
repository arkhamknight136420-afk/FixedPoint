// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FixedPointCharacter.generated.h"


class UCameraComponent;
class UCapsuleComponent;
class UPrimitiveComponent;
class UFixedPointInventoryComponent;

UCLASS()
class FIXEDPOINT_API AFixedPointCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	
	// LIFE CYCLE
	
	AFixedPointCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;


	// MOVEMENT
	
	
	void Move(const FVector2D& MovementInput);

	
	void SetSprintRequested(bool bRequested);
	bool IsSprintRequested() const;

	void ToggleCrouch();

	
	// INTERACTION
	
	void TryInteract();

	// INVENTORY

	void SelectInventorySlot(int32 Index);
	void CycleInventorySlot(int32 Direction);
	void UseEquippedItem();
	void DropHeldItem();

	UFUNCTION(BlueprintPure, Category = "Inventory")
	UFixedPointInventoryComponent* GetInventoryComponent() const
	{
		return InventoryComponent.Get();
	}

protected:
	
	// MOVEMENT
	
	void InitializeMovementComponent();


	
	// INTERACTION
	
	
	void InitializeInteractionCapsuleComponent();


	void BindInteractionCapsuleEvents();


	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void OnInteractionCapsuleBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void OnInteractionCapsuleEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);



	AActor* FindMostAlignedInteractableActor() const;

	UFUNCTION(Server, Reliable)
	void ServerTryInteract(AActor* RequestedTarget);

	UFUNCTION(Server, Reliable)
	void ServerSelectInventorySlot(int32 Index);

	UFUNCTION(Server, Reliable)
	void ServerCycleInventorySlot(int32 Direction);

	UFUNCTION(Server, Reliable)
	void ServerUseEquippedItem();

	UFUNCTION(Server, Reliable)
	void ServerDropHeldItem();

	bool IsValidInteractionTarget(const AActor* RequestedTarget) const;


	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Interaction")
	UCapsuleComponent* InteractionCapsule = nullptr;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Interaction")
	TArray<TObjectPtr<AActor>> AvailableInteractables;


	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	AActor* FocusedInteractable = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Validation", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float MaxInteractionDistance = 300.f;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Interaction|Validation",
		meta = (
			ClampMin = "-1.0",
			ClampMax = "1.0",
			UIMin = "-1.0",
			UIMax = "1.0"))
	float MinimumInteractionAlignment = 0.25f;

	//INVENTORY
	
	void InitializeInventoryComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UFixedPointInventoryComponent> InventoryComponent = nullptr;


private:

	
	// CAMERA
	


	void InitializeCameraComponent();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera",
		meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> PlayerCamera = nullptr;




	
	



};
