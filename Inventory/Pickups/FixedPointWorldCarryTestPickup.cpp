// FixedPointWorldCarryTestPickup.cpp
#include "FixedPointWorldCarryTestPickup.h"

#include "../FixedPointItemDefinition.h"
#include "../../Player/Characters/FixedPointCharacter.h"

void AFixedPointWorldCarryTestPickup::BeginPlay()
{
    Super::BeginPlay();

    const UFixedPointItemDefinition* Definition = GetItemDefinition();
    if (HasAuthority() && (!IsValid(Definition) ||
        Definition->CarryType != EFixedPointCarryType::WorldCarry))
    {
        UE_LOG(LogTemp, Warning, TEXT("%s needs a World Carry item definition."), *GetName());
    }
}

void AFixedPointWorldCarryTestPickup::Use_Implementation(
    AFixedPointCharacter* UsingCharacter)
{
    if (!HasAuthority() || !IsValid(UsingCharacter) ||
        UsingCharacter != GetCarrier() ||
        CarryState != EFixedPointPickupState::WorldCarried)
    {
        return;
    }

    UE_LOG(LogTemp, Log,
        TEXT("World Carry Use confirmed on server: %s used %s"),
        *GetNameSafe(UsingCharacter), *GetName());
}