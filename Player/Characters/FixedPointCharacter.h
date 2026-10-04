// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "FixedPointCharacter.generated.h"


class UCameraComponent;
class UCapsuleComponent;
class UPrimitiveComponent;
class UFixedPointInventoryComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogFixedPointCharacter, Log, All);

UCLASS()
class FIXEDPOINT_API AFixedPointCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	
	// LIFE CYCLE
	
	AFixedPointCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	//NETWORKING
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// MOVEMENT
	
	
	void Move(const FVector2D& MovementInput);

	
	void SetSprintRequested(bool bRequested);
	bool IsSprintRequested() const;

	void StartJump();
	void StopJump();

	void StartCrouch();
	void StopCrouch();

	void SelectInventorySlot1();
	void SelectInventorySlot2();
	void SelectInventorySlot3();

	void SelectNextInventorySlot();
	void SelectPreviousInventorySlot();

	void DropItemStarted();

	void StartUseItemPrimary();
	void StopUseItemPrimary();

	void StartUseItemSecondary();
	void StopUseItemSecondary();



	void ToggleCrouch();

	// WEIGHT
	float GetSpeedReductionMultiplier() const
	{
		return SpeedReductionMultiplier;
	}


	UFUNCTION(BlueprintCallable, Category = "Weight")
	void RefreshCarryWeight();

	
	// INTERACTION
	
	void TryInteract();

	//INVENTORY

	UFixedPointInventoryComponent* GetInventoryComponent()
	{
		return  InventoryComponent;
	}

protected:
	
	// MOVEMENT
	
	void InitializeMovementComponent();

	//ACTIONS


	//Weight


	// The total Weight the player is carrying tallying all there  held items in lbs
	UPROPERTY(VisibleAnywhere, Category = "Weight")
	float TotalCarryWeight = 0.f;

	// Maximum movement speed multiplier for slowdown. 1 means movement speed cannot be above 100% of its original value.
	UPROPERTY(EditDefaultsOnly, Category = "Weight")
	float MinSpeedReductionMultiplier = 1.f;

	// Minimum movement speed multiplier for slowdown. 0.6 means movement speed cannot fall below 60% of its original value.
	UPROPERTY(EditDefaultsOnly, Category = "Weight")
	float MaxSpeedReductionMultiplier = 0.6f;

	// Current movement speed multiplier for slowdown.
	UPROPERTY(Replicated, BlueprintReadonly, Category = "Weight")
	float SpeedReductionMultiplier = 1.f;

	// The minimum amount of additional weight the player is carrying from objects in there inventory
	UPROPERTY(EditDefaultsOnly, Category = "Weight")
	float MinCarryWeight = 0.f;

	// The maximum amount of additional weight the player is carrying from objects in there inventory
	UPROPERTY(EditDefaultsOnly, Category = "Weight")
	float MaxCarryWeight = 50.f;

	

	
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

	UFUNCTION(Server, Reliable)
	void ServerSelectInventorySlot(int32 SelectedIndex);

	UFUNCTION(Server, Reliable)
	void ServerCycleNextInventorySlot();

	UFUNCTION(Server, Reliable)
	void ServerCyclePreviousInventorySlot();

	UFUNCTION(Server, Reliable)
	void ServerDropItem();

	


	// CAMERA

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera",
		meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> PlayerCamera = nullptr;


private:
	//INVENTORY

	UPROPERTY()
	UFixedPointInventoryComponent* InventoryComponent = nullptr;
	
	// CAMERA
	

	void InitializeCameraComponent();





	
	
	



};
