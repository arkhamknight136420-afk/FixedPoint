// Fill out your copyright notice in the Description page of Project Settings.


#include "FixedPointFlashlight.h"
 

FInstancedStruct AFixedPointFlashlight::CaptureItemState() const
{
	FFixedPointFlashlightState FlashlightState;

	FlashlightState.Battery = Battery;

	FlashlightState.bOn = bOn;

	UE_LOG(LogFixedPointPickup, Log, TEXT("CaptureItemState() | Captured flashlight state | Battery: %f bOn : %s"), Battery, bOn ? TEXT("true") : TEXT("False"))

		return FInstancedStruct::Make(FlashlightState);

}

void AFixedPointFlashlight::UpdateItemState(FFixedPointInventoryEntry Entry)
{
	const FFixedPointFlashlightState* FlashlightUpdatedState = Entry.State.GetPtr<FFixedPointFlashlightState>();

	if (!FlashlightUpdatedState)
	{
		UE_LOG(LogFixedPointPickup, Warning, TEXT("CaptureItemState() | The pointer to the Struct of FlashlightUpdatedState is null "))
		return;

	}

	Battery = FlashlightUpdatedState->Battery;

	bOn = FlashlightUpdatedState->bOn;

	UE_LOG(LogFixedPointPickup, Log, TEXT("UpdateItemState() | Restored flashlight state | Battery: %f bOn : %s"), Battery, bOn ? TEXT("true") : TEXT("False"))

	
}