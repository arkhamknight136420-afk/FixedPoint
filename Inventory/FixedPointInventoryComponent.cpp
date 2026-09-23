#include "FixedPointInventoryComponent.h"

#include "FixedPointItemDefinition.h"
#include "Pickups/FixedPointPickup.h"
#include "../Player/Characters/FixedPointCharacter.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

UFixedPointInventoryComponent::UFixedPointInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	Slots.SetNum(SlotCount);
}

void UFixedPointInventoryComponent::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(
		UFixedPointInventoryComponent, Slots, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(
		UFixedPointInventoryComponent, SelectedSlot, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(
		UFixedPointInventoryComponent, WorldCarriedItem, COND_OwnerOnly);
}

void UFixedPointInventoryComponent::OnRep_InventoryState()
{
	// Temporary verification until the hotbar UI exists.
	UE_LOG(
		LogTemp,
		Log,
		TEXT("Owner inventory %s: [%s, %s, %s], selected %d, world carry %s"),
		*GetNameSafe(GetOwner()),
		*GetNameSafe(GetItemInSlot(0)),
		*GetNameSafe(GetItemInSlot(1)),
		*GetNameSafe(GetItemInSlot(2)),
		SelectedSlot + 1,
		*GetNameSafe(WorldCarriedItem.Get()));
}

AFixedPointPickup* UFixedPointInventoryComponent::GetItemInSlot(
	int32 Index) const
{
	return Slots.IsValidIndex(Index) ? Slots[Index].Get() : nullptr;
}

bool UFixedPointInventoryComponent::IsServerOwner() const
{
	return GetOwner() && GetOwner()->HasAuthority();
}

AFixedPointCharacter*
UFixedPointInventoryComponent::GetCharacterOwner() const
{
	return Cast<AFixedPointCharacter>(GetOwner());
}

int32 UFixedPointInventoryComponent::FindFreeSlot() const
{
	if (Slots.Num() != SlotCount)
	{
		return INDEX_NONE;
	}

	// If the player selected an empty slot, fill that slot first.
	if (Slots.IsValidIndex(SelectedSlot) &&
		!IsValid(GetItemInSlot(SelectedSlot)))
	{
		return SelectedSlot;
	}

	for (int32 Index = 0; Index < SlotCount; ++Index)
	{
		if (!IsValid(GetItemInSlot(Index)))
		{
			return Index;
		}
	}

	return INDEX_NONE;
}

bool UFixedPointInventoryComponent::HasHandsFullItem() const
{
	for (const TObjectPtr<AFixedPointPickup>& Item : Slots)
	{
		if (IsValid(Item.Get()) &&
			IsValid(Item->GetItemDefinition()) &&
			Item->GetItemDefinition()->CarryType ==
			EFixedPointCarryType::HandsFull)
		{
			return true;
		}
	}

	return false;
}

bool UFixedPointInventoryComponent::CanAcceptPickup(
	const AFixedPointPickup* Pickup) const
{
	if (!IsServerOwner() ||
		!IsValid(Pickup) ||
		!Pickup->IsAvailableInWorld() ||
		!IsValid(Pickup->GetItemDefinition()) ||
		!IsValid(GetCharacterOwner()))
	{
		return false;
	}

	const EFixedPointCarryType CarryType =
		Pickup->GetItemDefinition()->CarryType;

	if (CarryType == EFixedPointCarryType::WorldCarry)
	{
		return !IsValid(WorldCarriedItem.Get()) &&
			!HasHandsFullItem();
	}

	if (FindFreeSlot() == INDEX_NONE)
	{
		return false;
	}

	// A pocket item can still be stored when the player's hands are busy.
	return CarryType == EFixedPointCarryType::Pocketable ||
		(CarryType == EFixedPointCarryType::HandsFull &&
			!HasHandsFullItem() &&
			!IsValid(WorldCarriedItem.Get()));
}

bool UFixedPointInventoryComponent::TryAddPickup(
	AFixedPointPickup* Pickup)
{
	if (!CanAcceptPickup(Pickup))
	{
		return false;
	}

	AFixedPointCharacter* Character = GetCharacterOwner();

	const EFixedPointCarryType CarryType =
		Pickup->GetItemDefinition()->CarryType;

	if (CarryType == EFixedPointCarryType::WorldCarry)
	{
		WorldCarriedItem = Pickup;

		Pickup->SetCarriedBy(
			Character,
			EFixedPointPickupState::WorldCarried);

		RefreshEquippedItems();
	}
	else
	{
		const int32 FreeSlot = FindFreeSlot();
		Slots[FreeSlot] = Pickup;

		if (CarryType == EFixedPointCarryType::HandsFull ||
			SelectedSlot == INDEX_NONE)
		{
			SelectedSlot = FreeSlot;
		}

		Pickup->SetCarriedBy(
			Character,
			EFixedPointPickupState::Stored);

		RefreshEquippedItems();
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT("%s picked up %s"),
		*GetNameSafe(Character),
		*GetNameSafe(Pickup));

	return true;
}

void UFixedPointInventoryComponent::RefreshEquippedItems()
{
	if (!IsServerOwner())
	{
		return;
	}

	const bool bWorldCarry = IsValid(WorldCarriedItem.Get());

	for (int32 Index = 0; Index < SlotCount; ++Index)
	{
		AFixedPointPickup* Item = GetItemInSlot(Index);

		if (IsValid(Item))
		{
			Item->SetCarriedBy(
				GetCharacterOwner(),
				!bWorldCarry && Index == SelectedSlot
				? EFixedPointPickupState::Equipped
				: EFixedPointPickupState::Stored);
		}
	}
}

void UFixedPointInventoryComponent::SelectSlot(int32 Index)
{
	if (!IsServerOwner() ||
		Index < INDEX_NONE ||
		Index >= SlotCount ||
		IsValid(WorldCarriedItem.Get()) ||
		HasHandsFullItem())
	{
		return;
	}

	SelectedSlot = Index;
	RefreshEquippedItems();

	if (Index == INDEX_NONE)
	{
		UE_LOG(
			LogTemp,
			Log,
			TEXT("%s selected empty hands"),
			*GetNameSafe(GetCharacterOwner()));
	}
	else
	{
		UE_LOG(
			LogTemp,
			Log,
			TEXT("%s selected inventory slot %d (%s)"),
			*GetNameSafe(GetCharacterOwner()),
			Index + 1,
			*GetNameSafe(GetItemInSlot(Index)));
	}
}

void UFixedPointInventoryComponent::CycleSlot(int32 Direction)
{
	if (!IsServerOwner() ||
		(Direction != -1 && Direction != 1) ||
		IsValid(WorldCarriedItem.Get()) ||
		HasHandsFullItem())
	{
		return;
	}

	// INDEX_NONE is one empty-hands stop.
	// Empty physical slots are skipped.
	int32 Candidate = SelectedSlot;

	for (int32 Attempt = 0; Attempt <= SlotCount; ++Attempt)
	{
		Candidate += Direction;

		if (Candidate >= SlotCount)
		{
			Candidate = INDEX_NONE;
		}
		else if (Candidate < INDEX_NONE)
		{
			Candidate = SlotCount - 1;
		}

		if (Candidate == INDEX_NONE ||
			IsValid(GetItemInSlot(Candidate)))
		{
			if (Candidate != SelectedSlot)
			{
				SelectSlot(Candidate);
			}

			return;
		}
	}
}
void UFixedPointInventoryComponent::UseEquippedItem()
{
	if (!IsServerOwner())
	{
		return;
	}

	AFixedPointPickup* Item = IsValid(WorldCarriedItem.Get())
		? WorldCarriedItem.Get()
		: GetItemInSlot(SelectedSlot);

	if (IsValid(Item) &&
		Item->GetCarrier() == GetCharacterOwner() &&
		Item->IsEquippedOrWorldCarried())
	{
		// The item actor decides what Use does.
		Item->Use(GetCharacterOwner());
	}
}

void UFixedPointInventoryComponent::DropPickup(
	AFixedPointPickup* Pickup)
{
	if (!IsValid(Pickup) ||
		!IsValid(GetCharacterOwner()))
	{
		return;
	}

	AFixedPointCharacter* Character = GetCharacterOwner();
	const FVector Forward = Character->GetActorForwardVector();

	const FVector TraceStart =
		Character->GetActorLocation() +
		Forward * 120.f +
		FVector(0.f, 0.f, 100.f);

	const FVector TraceEnd =
		TraceStart - FVector(0.f, 0.f, 260.f);

	FCollisionQueryParams Params(
		SCENE_QUERY_STAT(DropPickup),
		false,
		Character);

	Params.AddIgnoredActor(Pickup);

	FHitResult GroundHit;

	const bool bFoundGround =
		GetWorld()->LineTraceSingleByChannel(
			GroundHit,
			TraceStart,
			TraceEnd,
			ECC_Visibility,
			Params);

	const float HalfHeight = Pickup->GetMeshHalfHeight();

	const FVector DropLocation = bFoundGround
		? GroundHit.ImpactPoint +
		FVector(0.f, 0.f, HalfHeight + 3.f)
		: Character->GetActorLocation() +
		Forward * 120.f;

	Pickup->DropAt(
		DropLocation,
		Character->GetActorRotation());
}

void UFixedPointInventoryComponent::DropHeldItem()
{
	if (!IsServerOwner())
	{
		return;
	}

	if (AFixedPointPickup* WorldItem = WorldCarriedItem.Get())
	{
		WorldCarriedItem = nullptr;
		DropPickup(WorldItem);
		RefreshEquippedItems();

		UE_LOG(
			LogTemp,
			Log,
			TEXT("%s dropped world-carried %s"),
			*GetNameSafe(GetCharacterOwner()),
			*GetNameSafe(WorldItem));

		return;
	}

	AFixedPointPickup* Item =
		GetItemInSlot(SelectedSlot);

	if (!IsValid(Item))
	{
		return;
	}

	Slots[SelectedSlot] = nullptr;
	DropPickup(Item);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("%s dropped %s"),
		*GetNameSafe(GetCharacterOwner()),
		*GetNameSafe(Item));

	SelectedSlot = INDEX_NONE;

	for (int32 Index = 0; Index < SlotCount; ++Index)
	{
		if (IsValid(GetItemInSlot(Index)))
		{
			SelectedSlot = Index;
			break;
		}
	}

	RefreshEquippedItems();
}

void UFixedPointInventoryComponent::HandleOwnerDeath()
{
	if (!IsServerOwner())
	{
		return;
	}

	if (AFixedPointPickup* Item = WorldCarriedItem.Get())
	{
		WorldCarriedItem = nullptr;

		if (IsValid(Item->GetItemDefinition()) &&
			Item->GetItemDefinition()->bDropOnDeath)
		{
			DropPickup(Item);
		}
		else
		{
			Item->Destroy();
		}
	}

	for (int32 Index = 0; Index < SlotCount; ++Index)
	{
		AFixedPointPickup* Item = GetItemInSlot(Index);
		Slots[Index] = nullptr;

		if (IsValid(Item))
		{
			if (IsValid(Item->GetItemDefinition()) &&
				Item->GetItemDefinition()->bDropOnDeath)
			{
				DropPickup(Item);
			}
			else
			{
				Item->Destroy();
			}
		}
	}

	SelectedSlot = INDEX_NONE;
}

void UFixedPointInventoryComponent::ClearForLoopReset()
{
	if (!IsServerOwner())
	{
		return;
	}

	if (IsValid(WorldCarriedItem.Get()))
	{
		WorldCarriedItem->Destroy();
	}

	WorldCarriedItem = nullptr;

	for (TObjectPtr<AFixedPointPickup>& Item : Slots)
	{
		if (IsValid(Item.Get()))
		{
			Item->Destroy();
		}

		Item = nullptr;
	}

	SelectedSlot = INDEX_NONE;
}