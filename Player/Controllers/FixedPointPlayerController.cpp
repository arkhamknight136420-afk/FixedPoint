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
	if (IA_InventorySlot1)
	{
		EnhancedInputComponent->BindAction(
			IA_InventorySlot1.Get(),
			ETriggerEvent::Started,
			this,
			&AFixedPointPlayerController::HandleInventorySlot1);
	}

	if (IA_InventorySlot2)
	{
		EnhancedInputComponent->BindAction(
			IA_InventorySlot2.Get(),
			ETriggerEvent::Started,
			this,
			&AFixedPointPlayerController::HandleInventorySlot2);
	}

	if (IA_InventorySlot3)
	{
		EnhancedInputComponent->BindAction(
			IA_InventorySlot3.Get(),
			ETriggerEvent::Started,
			this,
			&AFixedPointPlayerController::HandleInventorySlot3);
	}

	if (IA_UseItem)
	{
		EnhancedInputComponent->BindAction(
			IA_UseItem.Get(),
			ETriggerEvent::Started,
			this,
			&AFixedPointPlayerController::HandleUseItem);
	}

	if (IA_DropItem)
	{
		EnhancedInputComponent->BindAction(
			IA_DropItem.Get(),
			ETriggerEvent::Started,
			this,
			&AFixedPointPlayerController::HandleDropItem);
	}

	if (IA_PreviousInventorySlot)
	{
		EnhancedInputComponent->BindAction(
			IA_PreviousInventorySlot.Get(),
			ETriggerEvent::Started,
			this,
			&AFixedPointPlayerController::HandlePreviousInventorySlot);
	}

	if (IA_NextInventorySlot)
	{
		EnhancedInputComponent->BindAction(
			IA_NextInventorySlot.Get(),
			ETriggerEvent::Started,
			this,
			&AFixedPointPlayerController::HandleNextInventorySlot);
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

void AFixedPointPlayerController::HandleInventorySlot1()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->SelectInventorySlot(0);
	}
}

void AFixedPointPlayerController::HandleInventorySlot2()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->SelectInventorySlot(1);
	}
}

void AFixedPointPlayerController::HandleInventorySlot3()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->SelectInventorySlot(2);
	}
}

void AFixedPointPlayerController::HandlePreviousInventorySlot()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->CycleInventorySlot(-1);
	}
}

void AFixedPointPlayerController::HandleNextInventorySlot()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->CycleInventorySlot(1);
	}
}

void AFixedPointPlayerController::HandleUseItem()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		UE_LOG(LogTemp, Log, TEXT("Use item input received for %s"),
			*GetNameSafe(ControlledCharacter));

		ControlledCharacter->UseEquippedItem();
	}
}
void AFixedPointPlayerController::HandleDropItem()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->DropHeldItem();
	}
}


AFixedPointCharacter* AFixedPointPlayerController::GetFixedPointCharacter() const
{
	return Cast<AFixedPointCharacter>(GetPawn());
}
