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
			&AFixedPointPlayerController::Input_Look);
	}

	if (ensureMsgf(IA_Move != nullptr, TEXT("IA_Move is not assigned on %s."), *GetName()))
	{
		EnhancedInputComponent->BindAction(
			IA_Move.Get(), ETriggerEvent::Triggered, this,
			&AFixedPointPlayerController::InputMove);
	}

	if (ensureMsgf(IA_Jump != nullptr, TEXT("IA_Jump is not assigned on %s."), *GetName()))
	{
		EnhancedInputComponent->BindAction(
			IA_Jump.Get(), ETriggerEvent::Started, this,
			&AFixedPointPlayerController::Input_Jump_Started);

		EnhancedInputComponent->BindAction(
			IA_Jump.Get(), ETriggerEvent::Completed, this,
			&AFixedPointPlayerController::Input_Jump_Ended);

		EnhancedInputComponent->BindAction(
			IA_Jump.Get(), ETriggerEvent::Canceled, this,
			&AFixedPointPlayerController::Input_Jump_Ended);
	}

	if (ensureMsgf(IA_Sprint != nullptr, TEXT("IA_Sprint is not assigned on %s."), *GetName()))
	{
		EnhancedInputComponent->BindAction(
			IA_Sprint.Get(), ETriggerEvent::Started, this,
			&AFixedPointPlayerController::Input_Sprint_Started);

		EnhancedInputComponent->BindAction(
			IA_Sprint.Get(), ETriggerEvent::Completed, this,
			&AFixedPointPlayerController::Input_Sprint_Ended);

		EnhancedInputComponent->BindAction(
			IA_Sprint.Get(), ETriggerEvent::Canceled, this,
			&AFixedPointPlayerController::Input_Sprint_Ended);
	}

	if (ensureMsgf(IA_Crouch != nullptr, TEXT("IA_Crouch is not assigned on %s."), *GetName()))
	{
		EnhancedInputComponent->BindAction(
			IA_Crouch.Get(), ETriggerEvent::Started, this,
			&AFixedPointPlayerController::Input_Crouch_Started);

		EnhancedInputComponent->BindAction(
			IA_Crouch.Get(), ETriggerEvent::Completed, this,
			&AFixedPointPlayerController::Input_Crouch_Ended);

		EnhancedInputComponent->BindAction(
			IA_Crouch.Get(), ETriggerEvent::Canceled, this,
			&AFixedPointPlayerController::Input_Crouch_Ended);
	}

	if (ensureMsgf(IA_Interact != nullptr, TEXT("IA_Interact is not assigned on %s."), *GetName()))
	{
		EnhancedInputComponent->BindAction(
			IA_Interact.Get(), ETriggerEvent::Started, this,
			&AFixedPointPlayerController::Input_Interact);
	}

	if (ensureMsgf(IA_DropItem != nullptr, TEXT("IA_DropItem is not assigned on %s."), *GetName()))
	{
		EnhancedInputComponent->BindAction(
			IA_DropItem.Get(), ETriggerEvent::Started, this,
			&AFixedPointPlayerController::Input_DropItem_Started);
	}

	if (ensureMsgf(IA_InventorySlot1 != nullptr, TEXT("IA_InventorySlot1 is not assigned on %s."), *GetName()))
	{
		EnhancedInputComponent->BindAction(
			IA_InventorySlot1.Get(), ETriggerEvent::Started, this,
			&AFixedPointPlayerController::Input_InventorySlot1);
	}

	if (ensureMsgf(IA_InventorySlot2 != nullptr, TEXT("IA_InventorySlot2 is not assigned on %s."), *GetName()))
	{
		EnhancedInputComponent->BindAction(
			IA_InventorySlot2.Get(), ETriggerEvent::Started, this,
			&AFixedPointPlayerController::Input_InventorySlot2);
	}

	if (ensureMsgf(IA_InventorySlot3 != nullptr, TEXT("IA_InventorySlot3 is not assigned on %s."), *GetName()))
	{
		EnhancedInputComponent->BindAction(
			IA_InventorySlot3.Get(), ETriggerEvent::Started, this,
			&AFixedPointPlayerController::Input_InventorySlot3);
	}

	if (ensureMsgf(IA_NextInventorySlot != nullptr, TEXT("IA_NextInventorySlot is not assigned on %s."), *GetName()))
	{
		EnhancedInputComponent->BindAction(
			IA_NextInventorySlot.Get(), ETriggerEvent::Started, this,
			&AFixedPointPlayerController::Input_NextInventorySlot);
	}

	if (ensureMsgf(IA_PreviousInventorySlot != nullptr, TEXT("IA_PreviousInventorySlot is not assigned on %s."), *GetName()))
	{
		EnhancedInputComponent->BindAction(
			IA_PreviousInventorySlot.Get(), ETriggerEvent::Started, this,
			&AFixedPointPlayerController::Input_PreviousInventorySlot);
	}

	if (ensureMsgf(IA_UseItemPrimary != nullptr, TEXT("IA_UseItemPrimary is not assigned on %s."), *GetName()))
	{
		EnhancedInputComponent->BindAction(
			IA_UseItemPrimary.Get(), ETriggerEvent::Started, this,
			&AFixedPointPlayerController::Input_UseItemPrimary_Started);

		EnhancedInputComponent->BindAction(
			IA_UseItemPrimary.Get(), ETriggerEvent::Completed, this,
			&AFixedPointPlayerController::Input_UseItemPrimary_Ended);
	}

	if (ensureMsgf(IA_UseItemSecondary != nullptr, TEXT("IA_Sprint is not assigned on %s."), *GetName()))
	{
		EnhancedInputComponent->BindAction(
			IA_UseItemSecondary.Get(), ETriggerEvent::Started, this,
			&AFixedPointPlayerController::Input_UseItemSecondary_Started);

		EnhancedInputComponent->BindAction(
			IA_UseItemSecondary.Get(), ETriggerEvent::Completed, this,
			&AFixedPointPlayerController::Input_UseItemSecondary_Ended);
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

void AFixedPointPlayerController::Input_Look(const FInputActionValue& Value)
{
	const FVector2D LookInput = Value.Get<FVector2D>();

	AddYawInput(LookInput.X);
	AddPitchInput(LookInput.Y);
}

void AFixedPointPlayerController::InputMove(const FInputActionValue& Value)
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->Move(Value.Get<FVector2D>());
	}
}

void AFixedPointPlayerController::Input_Jump_Started()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->StartJump();
	}
}

void AFixedPointPlayerController::Input_Jump_Ended()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->StopJump();
	}
}

void AFixedPointPlayerController::Input_Sprint_Started()
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

void AFixedPointPlayerController::Input_Sprint_Ended()
{
	if (!bSprintToggleEnabled)
	{
		if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
		{
			ControlledCharacter->SetSprintRequested(false);
		}
	}
}

void AFixedPointPlayerController::Input_Crouch_Started()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		if (bCrouchToggleEnabled)
		{
			ControlledCharacter->ToggleCrouch();
		}
		else
		{
			ControlledCharacter->StartCrouch();
		}
	}
}

void AFixedPointPlayerController::Input_Crouch_Ended()
{
	if (!bCrouchToggleEnabled)
	{
		if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
		{
			ControlledCharacter->UnCrouch();
		}
	}
}

void AFixedPointPlayerController::Input_Interact()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->TryInteract();
	}
}



void AFixedPointPlayerController::Input_InventorySlot1()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->SelectInventorySlot1();
	}
}

void AFixedPointPlayerController::Input_InventorySlot2()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->SelectInventorySlot2();
	}
}

void AFixedPointPlayerController::Input_InventorySlot3()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->SelectInventorySlot3();
	}
}

void AFixedPointPlayerController::Input_NextInventorySlot()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->SelectNextInventorySlot();
	}
}

void AFixedPointPlayerController::Input_PreviousInventorySlot()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->SelectPreviousInventorySlot();
	}
}

void AFixedPointPlayerController::Input_DropItem_Started()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->DropItemStarted();
	}
}

void AFixedPointPlayerController::Input_UseItemPrimary_Started()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->StartUseItemPrimary();
	}
}

void AFixedPointPlayerController::Input_UseItemPrimary_Ended()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->StopUseItemPrimary();
	}
}

void AFixedPointPlayerController::Input_UseItemSecondary_Started()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->StartUseItemSecondary();
	}
}

void AFixedPointPlayerController::Input_UseItemSecondary_Ended()
{
	if (AFixedPointCharacter* ControlledCharacter = GetFixedPointCharacter())
	{
		ControlledCharacter->StopUseItemSecondary();
	}
}

//GETTER

AFixedPointCharacter* AFixedPointPlayerController::GetFixedPointCharacter() const
{
	return Cast<AFixedPointCharacter>(GetPawn());
}
