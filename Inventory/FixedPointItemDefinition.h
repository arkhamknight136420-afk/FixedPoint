// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FixedPointItemDefinition.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EFixedPointCarryType : uint8
{
	Pocketable UMETA(DisplayName = "Pocketable"),
	HandsFull UMETA(DisplayName = "Hands Full"),
	WorldCarry UMETA(DisplayName = "World Carry")
};

UCLASS(BlueprintType)
class FIXEDPOINT_API UFixedPointItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Carrying")
	float Weight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Carrying")
	EFixedPointCarryType CarryType = EFixedPointCarryType::Pocketable;
};