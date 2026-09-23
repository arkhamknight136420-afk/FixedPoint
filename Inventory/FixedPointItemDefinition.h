#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FixedPointItemDefinition.generated.h"

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
    // Every copy using this definition has the same weight.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Carrying",
        meta = (ClampMin = "0.0", UIMin = "0.0", DisplayName = "Weight (lb)"))
    float Weight = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Carrying")
    EFixedPointCarryType CarryType = EFixedPointCarryType::Pocketable;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Carrying")
    bool bDropOnDeath = true;
};