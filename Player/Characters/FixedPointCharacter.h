// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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
