// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FixedPointCharacter.generated.h"

class UCameraComponent;
class UCapsuleComponent;
class UPrimitiveComponent;

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



protected:
	
	// Movement
	
	void InitializeMovementComponent();


	
	// INTERACTION
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void InitializeInteractionCapsuleComponent();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void BindInteractionCapsuleEvents();


	UFUNCTION()
	void OnInteractionCapsuleBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractionCapsuleEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);



	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Interaction")
	UCapsuleComponent* InteractionCapsule = nullptr;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Interaction")
	TArray<TObjectPtr<AActor>> AvailableInteractables;






private:

	//=====================================================
	// CAMERA
	//=====================================================

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void InitializeCameraComponent();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera",
		meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> PlayerCamera = nullptr;

	// Temporary intent storage. This does not change movement speed and is not
	// replicated; the custom movement component will replace it next.


	
	



};
