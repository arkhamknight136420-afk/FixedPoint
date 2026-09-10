// Fill out your copyright notice in the Description page of Project Settings.

#include "FixedPointCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "../MovementComponents/FixedPointMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"

AFixedPointCharacter::AFixedPointCharacter(
	const FObjectInitializer& ObjectInitializer)
	: Super(
		ObjectInitializer.SetDefaultSubobjectClass<UFixedPointMovementComponent>(
			ACharacter::CharacterMovementComponentName))
{
	// Standard first-person rotation behavior: yaw follows the controller,
	// while pitch is left to the camera rather than tilting the capsule.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->bOrientRotationToMovement = false;
	MovementComponent->GetNavAgentPropertiesRef().bCanCrouch = true;

	FirstPersonCamera =
		CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));

	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());

	// Approximately eye height for the default Character capsule.
	// This can be tuned later from the character Blueprint.
	FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f));

	// Pitch and yaw come from the owning PlayerController's control rotation.
	FirstPersonCamera->bUsePawnControlRotation = true;
}

void AFixedPointCharacter::Move(const FVector2D& MovementInput)
{
	if (!Controller)
	{
		return;
	}

	// Ignore control pitch and roll so looking up or down never makes forward
	// movement push the character into the floor or into the air.
	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);

	const FVector ForwardDirection =
		FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	const FVector RightDirection =
		FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementInput.Y);
	AddMovementInput(RightDirection, MovementInput.X);
}

void AFixedPointCharacter::SetSprintRequested(const bool bRequested)
{
	UFixedPointMovementComponent* MovementComponent =
		CastChecked<UFixedPointMovementComponent>(GetCharacterMovement());

	MovementComponent->SetWantsToSprint(bRequested);
}

bool AFixedPointCharacter::IsSprintRequested() const
{
	const UFixedPointMovementComponent* MovementComponent =
		CastChecked<UFixedPointMovementComponent>(GetCharacterMovement());

	return MovementComponent->WantsToSprint();
}
void AFixedPointCharacter::ToggleCrouch()
{
	if (IsCrouched())
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AFixedPointCharacter::TryInteract()
{
	// Intentionally empty in this pass. This function will initiate the
	// character-owned, server-validated interaction flow later.
}
