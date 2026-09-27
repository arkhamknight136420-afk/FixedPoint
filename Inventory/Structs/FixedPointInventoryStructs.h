// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StructUtils/InstancedStruct.h"
#include "CoreMinimal.h"
#include "FixedPointInventoryStructs.generated.h"


class UFixedPointItemDefinition;

USTRUCT(BlueprintType)
struct FFixedPointInventoryEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    TObjectPtr<UFixedPointItemDefinition> Definition = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    FInstancedStruct State;

};

USTRUCT(BlueprintType)
struct FFixedPointFlashlightState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    float Battery = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    bool bOn = false;
};


/* to create the structs for the stored data 
* 
USTRUCT(BlueprintType)
struct FFixedPoint(object name)State
{
    GENERATED_BODY()

    UPROPERTY()
    VariableType* VariableName;

    UPROPERTY()
    VariableType* VariableName;

};

*/


