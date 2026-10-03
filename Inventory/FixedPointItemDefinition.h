// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FixedPointItemDefinition.generated.h"

/**
 * 
 */

class AFixedPointPickup;



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

	/*The weight of this item in lbs*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Carrying")
	float Weight = 0.0f;

	/*The carry type of this item Definining its inventory behaviour*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Carrying")
	EFixedPointCarryType CarryType = EFixedPointCarryType::Pocketable;

	/*The Physical Item Blueprint Spawned Into The World*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|World")
	TSubclassOf<AFixedPointPickup> WorldPickupClass;




};