// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Pickups/FixedPointPickup.h"
#include "FixedPointFlashlight.generated.h"

/**
 * 
 */
UCLASS()
class FIXEDPOINT_API AFixedPointFlashlight : public AFixedPointPickup
{
	GENERATED_BODY()
	

public:

	virtual FInstancedStruct CaptureItemState() const override;

	virtual void UpdateItemState(FFixedPointInventoryEntry Entry) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float Battery = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bOn = false;


private:

	

};
