// Fill out your copyright notice in the Description page of Project Settings.


#include "FixedPointFlashlight.h"
#include "../../Player/Characters/FixedPointCharacter.h"

void AFixedPointFlashlight::Use_Implementation(
	AFixedPointCharacter* UsingCharacter)
{
	if (!HasAuthority() ||
		!IsValid(UsingCharacter) ||
		UsingCharacter != GetCarrier() ||
		CarryState != EFixedPointPickupState::Equipped)
	{
		return;
	}

	UE_LOG(LogTemp, Log,
		TEXT("Flashlight Use confirmed on server: %s used %s"),
		*GetNameSafe(UsingCharacter),
		*GetName());
}
