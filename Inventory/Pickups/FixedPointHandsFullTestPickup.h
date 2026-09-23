// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FixedPointPickup.h"
#include "FixedPointHandsFullTestPickup.generated.h"

UCLASS()
class FIXEDPOINT_API AFixedPointHandsFullTestPickup : public AFixedPointPickup
{
    GENERATED_BODY()

public:
    virtual void Use_Implementation(AFixedPointCharacter* UsingCharacter) override;

protected:
    virtual void BeginPlay() override;
};