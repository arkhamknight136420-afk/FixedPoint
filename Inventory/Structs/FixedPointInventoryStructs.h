// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StructUtils/InstancedStruct.h"
#include "CoreMinimal.h"
#include "FixedPointInventoryStructs.generated.h"

 
class UFixedPointItemDefinition;


USTRUCT(BlueprintType)
struct FIXEDPOINT_API FFixedPointInventorySummary
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    TArray<TObjectPtr<UFixedPointItemDefinition>> SlotDefinitons;

    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    TObjectPtr<UFixedPointItemDefinition> WorldCarryDefinition = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    TObjectPtr<UFixedPointItemDefinition> HeldDefinition = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    int32 SelectedSlotIndex = INDEX_NONE;

    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    bool bCanSwapHeldItems = true;

    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    float TotalCarryWeight = 0.0f;

    // In this implementation, revision zero is the initial default. Publishing creates revision one, then two, and so on.
    //More precisely, this is a publication counter : the proposed function increments it whenever called.It does not compare
    // all fields to prove that their contents changed. It also does not identify a pickup request.A new revision could result from selecting a slot,
    // picking up an item, or dropping something.

    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    int32 Revision = 0;


};


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


