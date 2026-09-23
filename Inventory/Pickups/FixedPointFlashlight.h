// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Pickups/FixedPointPickup.h"
#include "FixedPointFlashlight.generated.h"

/** Confirms that Use reaches the equipped item on the server. */
UCLASS()
class FIXEDPOINT_API AFixedPointFlashlight : public AFixedPointPickup
{
	GENERATED_BODY()

public:
	virtual void Use_Implementation(AFixedPointCharacter* UsingCharacter) override;
};

