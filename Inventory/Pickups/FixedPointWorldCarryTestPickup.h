// FixedPointWorldCarryTestPickup.h
#pragma once

#include "CoreMinimal.h"
#include "FixedPointPickup.h"
#include "FixedPointWorldCarryTestPickup.generated.h"

UCLASS()
class FIXEDPOINT_API AFixedPointWorldCarryTestPickup : public AFixedPointPickup
{
    GENERATED_BODY()

public:
    virtual void Use_Implementation(AFixedPointCharacter* UsingCharacter) override;

protected:
    virtual void BeginPlay() override;
};