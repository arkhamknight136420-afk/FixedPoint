// FixedPointHandsFullTestPickup.cpp
#include "FixedPointHandsFullTestPickup.h"

#include "../FixedPointItemDefinition.h"
#include "../../Player/Characters/FixedPointCharacter.h"

void AFixedPointHandsFullTestPickup::BeginPlay()
{
    Super::BeginPlay();

    const UFixedPointItemDefinition* Definition = GetItemDefinition();
    if (HasAuthority() && (!IsValid(Definition) ||
        Definition->CarryType != EFixedPointCarryType::HandsFull))
    {
        UE_LOG(LogTemp, Warning, TEXT("%s needs a Hands Full item definition."), *GetName());
    }
}

void AFixedPointHandsFullTestPickup::Use_Implementation(
    AFixedPointCharacter* UsingCharacter)
{
    if (!HasAuthority() || !IsValid(UsingCharacter) ||
        UsingCharacter != GetCarrier() ||
        CarryState != EFixedPointPickupState::Equipped)
    {
        return;
    }

    UE_LOG(LogTemp, Log,
        TEXT("Hands Full Use confirmed on server: %s used %s"),
        *GetNameSafe(UsingCharacter), *GetName());
}