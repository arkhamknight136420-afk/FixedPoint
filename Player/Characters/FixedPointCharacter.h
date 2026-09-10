// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FixedPointCharacter.generated.h"

class UCameraComponent;

UCLASS()
class FIXEDPOINT_API AFixedPointCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFixedPointCharacter(const FObjectInitializer& ObjectInitializer);

	// Receives device-independent movement intent from the PlayerController.
	void Move(const FVector2D& MovementInput);

	// This is only the controller-facing API for now. The next movement pass
	// will move sprint intent into the custom predicted movement component.
	void SetSprintRequested(bool bRequested);
	bool IsSprintRequested() const;

	void ToggleCrouch();

	// Interaction behavior will be implemented after the interaction system
	// is designed. The controller can already forward the input here
	void TryInteract();

private:

	//=====================================================
	// CAMERA
	//=====================================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera",
		meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera = nullptr;

	// Temporary intent storage. This does not change movement speed and is not
	// replicated; the custom movement component will replace it next.
};
