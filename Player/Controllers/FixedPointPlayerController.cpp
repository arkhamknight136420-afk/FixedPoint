// Fill out your copyright notice in the Description page of Project Settings.

#include "FixedPointPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "../Characters/FixedPointCharacter.h"
#include "InputAction.h"
#include "InputMappingContext.h"

void AFixedPointPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// A dedicated server has PlayerControllers, but it has no local player
	// whose physical input needs an Input Mapping Context.
	if (!IsLocalController())
	{
		return;
	}

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!ensureMsgf(LocalPlayer, TEXT("%s has no LocalPlayer."), *GetName()))
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
		LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if (!ensureMsgf(InputSubsystem, TEXT("Enhanced Input subsystem was not available for %s."), *GetName()))
	{
		return;
	}

	if (ensureMsgf(PlayerMappingContext != nullptr,
		TEXT("PlayerMappingContext is not assigned on %s."), *GetName()))
	{
		InputSubsystem->AddMappingContext(PlayerMappingContext.Get(), PlayerMappingPriority);
	}
}

void AFixedPointPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent =
		Cast<UEnhancedInputComponent>(InputComponent);

	if (!ensureMsgf(EnhancedInputComponent,
		TEXT("%s requires an Enhanced Input Component."), *GetName()))
	{
		return;
	}

	if (ensureMsgf(IA_Look != nullptr, TEXT("IA_Look is not assigned on %s."), *GetName()))
	{
		EnhancedInputComponent->BindAction(
			IA_Look.Get(), ETriggerEvent::Triggered, this,
			&AFixedPointPlayerController::HandleLook);
	}

	if (ensureMsgf(IA_Move != nullptr, TEXT("IA_Move is not assigned on %s."), *GetName()))
	{
		EnhancedInputComponent->BindAction(
			IA_Move.Get(), ETriggerEvent::Triggered, this,
			&AFixedPointPlayerController::HandleMove);
	}

	if (ensureMsgf(IA_Jump != nullptr, TEXT("IA_Jump is not assigned on %s."), *GetName()))
	{
		EnhancedInputComponent->BindAction(
			IA_Jump.Get(), ETriggerEvent::Started, this,
			&AFixedPointPlayerController::HandleJumpStarted);

		EnhancedInputComponent->BindAction(
			IA_Jump.Get(), ETriggerEvent::Completed, this,
			&AFixedPointPlayerController::HandleJumpEnded);

		EnhancedInputComponent->BindAction(
			IA_Jump.Get(), ETriggerEvent::Canceled, this,
			&AFixedPointPlayerController::HandleJumpEnded);
	}

	if (ensureMsgf(IA_Sprint != nullptr, TEXT("IA_Sprint is not assigned on %s."), *GetName()))
	{
		EnhancedInputComponent->BindAction(
			IA_Sprint.Get(), ETriggerEvent::Started, this,
			&AFixedPointPlayerController::HandleSprintStarted);

		EnhancedInputComponent->BindAction(
			IA_Sprint.Get(), ETriggerEvent::Completed, this,
			&AFixedPointPlayerController::HandleSprintEnded);

		EnhancedInputComponent->BindAction(
			IA_Sprint.Get(), ETriggerEvent::Canceled, this,
			&AFixedPointPlayerController::HandleSprintEnded);
	}

	if (ensureMsgf(IA_Crouch != nullptr, TEXT("IA_Crouch is not assigned on %s."), *GetName()))
	{
		EnhancedInputComponent->BindAction(
			IA_Crouch.Get(), ETriggerEvent::Started, this,
			&AFixedPointPlayerController::HandleCrouchStarted);

		EnhancedInputComponent->BindAction(
			IA_Crouch.Get(), ETriggerEvent::Completed, this,
			&AFixedPointPlayerController::HandleCrouchEnded);

		EnhancedInputComponent->BindAction(
			IA_Crouch.Get(), ETriggerEvent::Canceled, this,
			&AFixedPointPlayerController::HandleCrouchEnded);
	}

	if (ensureMsgf(IA_Interact != nullptr, TEXT("IA_Interact is not assigned on %s."), *GetName()))
	{
		EnhancedInputComponent->BindAction(
			IA_Interact.Get(), ETriggerEvent::Started, this,
			&AFixedPointPlayerController::HandleInteract);
	}
}

void AFixedPointPlayerController::SetCrouchToggleEnabled(const bool bEnabled)
{
	bCrouchToggleEnabled = bEnabled;
}

bool AFixedPointPlayerController::IsCrouchToggleEnabled() const
{
	return bCrouchToggleEnabled;
}

void AFixedPointPlayerController::SetSprintToggleEnabled(const bool bEnabled)
{
	bSprintToggleEnabled = bEnabled;
}

bool AFixedPointPlayerController::IsSprintToggleEnabled() const
{
	return bSprintToggleEnabled;
}

void AFixedPointPlayerController::HandleLook(const FInputActionValue& Value)
{
	const FVector2D LookInput = Value.Get<FVector2D>();

	AddYawInput(LookInput.X);
	AddPitchInput(LookInput.Y);
}

void AFixedPointPlayerController::HandleMove(const FInputActionValue& Value)
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->Move(Value.Get<FVector2D>());
	}
}

void AFixedPointPlayerController::HandleJumpStarted()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->Jump();
	}
}

void AFixedPointPlayerController::HandleJumpEnded()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->StopJumping();
	}
}

void AFixedPointPlayerController::HandleSprintStarted()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		if (bSprintToggleEnabled)
		{
			ControlledCharacter->SetSprintRequested(!ControlledCharacter->IsSprintRequested());
		}
		else
		{
			ControlledCharacter->SetSprintRequested(true);
		}
	}
}

void AFixedPointPlayerController::HandleSprintEnded()
{
	if (!bSprintToggleEnabled)
	{
		if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
		{
			ControlledCharacter->SetSprintRequested(false);
		}
	}
}

void AFixedPointPlayerController::HandleCrouchStarted()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		if (bCrouchToggleEnabled)
		{
			ControlledCharacter->ToggleCrouch();
		}
		else
		{
			ControlledCharacter->Crouch();
		}
	}
}

void AFixedPointPlayerController::HandleCrouchEnded()
{
	if (!bCrouchToggleEnabled)
	{
		if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
		{
			ControlledCharacter->UnCrouch();
		}
	}
}

void AFixedPointPlayerController::HandleInteract()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->TryInteract();
	}
}

AFixedPointCharacter* AFixedPointPlayerController::GetFixedPointCharacter() const
{
	return Cast<AFixedPointCharacter>(GetPawn());
}
