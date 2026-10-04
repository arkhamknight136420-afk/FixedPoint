// Fill out your copyright notice in the Description page of Project Settings.


#include "FixedPointInventoryComponent.h"
#include "Pickups/FixedPointPickup.h"
#include "GameFramework/Actor.h"
#include "../Inventory/FixedPointItemDefinition.h"
#include "Structs/FixedPointInventoryStructs.h"
#include "../Player/Characters/FixedPointCharacter.h"

DEFINE_LOG_CATEGORY(LogFixedPointInventory);


// LIFECYCLE

UFixedPointInventoryComponent::UFixedPointInventoryComponent()
{
	
	
}

void UFixedPointInventoryComponent::BeginPlay()
{

	OwningFixedPointCharacter = Cast<AFixedPointCharacter>(GetOwner());

	if (!IsValid(OwningFixedPointCharacter))
	{
		UE_LOG(LogFixedPointInventory, Error, TEXT("UFixedPointInventoryComponent::BeginPlay() | Owning Character is not valid"))
	}
}

void UFixedPointInventoryComponent::TryAddItem(
	AFixedPointPickup* NewItem)
{
	AActor* Owner = GetOwner();

	if (!IsValid(Owner) ||
		!Owner->HasAuthority() ||
		!IsValid(OwningFixedPointCharacter) ||
		!IsValid(NewItem))
	{
		return;
	}

	UFixedPointItemDefinition* ItemDefinition =
		NewItem->GetItemDefinition();

	if (!IsValid(ItemDefinition))
	{
		return;
	}

	const EFixedPointCarryType NewCarryType =
		ItemDefinition->CarryType;

	const bool bIsWorldCarry =
		NewCarryType == EFixedPointCarryType::WorldCarry;

	// A WorldCarry item prevents picking up another item.
	if (IsValid(WorldCarryEntry.Definition))
	{
		UE_LOG(
			LogFixedPointInventory,
			Warning,
			TEXT("Pickup rejected: already holding a WorldCarry item"));

		return;
	}

	// While holding HandsFull, only Pocketable pickups are allowed.
	if (IsValid(CurrentHeldItem) &&
		CurrentHeldItem->CarryType == EFixedPointCarryType::HandsFull &&
		NewCarryType != EFixedPointCarryType::Pocketable)
	{
		UE_LOG(
			LogFixedPointInventory,
			Warning,
			TEXT("Pickup rejected: holding HandsFull; new item is not Pocketable"));

		return;
	}

	int32 SlotIndex = INDEX_NONE;

	if (!bIsWorldCarry)
	{
		for (int32 Index = 0; Index < InventoryItems.Num(); ++Index)
		{
			if (!IsValid(InventoryItems[Index].Definition))
			{
				SlotIndex = Index;
				break;
			}
		}

		if (SlotIndex == INDEX_NONE &&
			InventoryItems.Num() >= MaxInventoryItems)
		{
			UE_LOG(
				LogFixedPointInventory,
				Warning,
				TEXT("Pickup rejected: inventory is full"));

			return;
		}
	}

	// Claim only after checking carry restrictions and capacity.
	if (!NewItem->TryClaim())
	{
		UE_LOG(
			LogFixedPointInventory,
			Warning,
			TEXT("Pickup rejected: item was already claimed"));

		return;
	}

	FFixedPointInventoryEntry Entry;
	Entry.Definition = ItemDefinition;
	Entry.State = NewItem->CaptureItemState();

	if (bIsWorldCarry)
	{
		WorldCarryEntry = Entry;
	}
	else if (SlotIndex != INDEX_NONE)
	{
		InventoryItems[SlotIndex] = Entry;
	}
	else
	{
		SlotIndex = InventoryItems.Add(Entry);
	}

	HandleCarryType(Entry, SlotIndex);

	OwningFixedPointCharacter->RefreshCarryWeight();

	UE_LOG(
		LogFixedPointInventory,
		Log,
		TEXT("Picked up: %s | Inventory array size: %d | WorldCarry: %s"),
		*GetNameSafe(ItemDefinition),
		InventoryItems.Num(),
		*GetNameSafe(WorldCarryEntry.Definition.Get()));

	NewItem->Destroy();
}

bool UFixedPointInventoryComponent::CanDropItem()
{
	AActor* Owner = GetOwner();

	if (!IsValid(Owner) ||
		!Owner->HasAuthority() ||
		!IsValid(CurrentHeldItem))
	{
		return false;
	}

	if (!CurrentHeldItem->WorldPickupClass)
	{
		return false;
	}


	if (CurrentHeldItem->CarryType == EFixedPointCarryType::WorldCarry)
	{
		// return true if the item were holding is the same as the world carry entry item definition
		return WorldCarryEntry.Definition == CurrentHeldItem;
	}

	if (!InventoryItems.IsValidIndex(CurrentInventoryIndex))
	{
		return false;
	}

	// return true if the current inventory indexes definition is the same as the current held items
	return InventoryItems[CurrentInventoryIndex].Definition ==
		CurrentHeldItem;
}

void UFixedPointInventoryComponent::DropItem()
{
	if (!CanDropItem())
	{
		return;
	}

	const bool bDroppingWorldCarry =
		CurrentHeldItem->CarryType == EFixedPointCarryType::WorldCarry;

	const int32 DroppedIndex = CurrentInventoryIndex;

	const FFixedPointInventoryEntry Entry =
		bDroppingWorldCarry
		? WorldCarryEntry
		: InventoryItems[DroppedIndex];

	UWorld* CurrentWorld = GetWorld();

	if (!IsValid(CurrentWorld))
	{
		UE_LOG(
			LogFixedPointInventory,
			Warning,
			TEXT("DropItem: world is invalid"));

		return;
	}

	const FTransform SpawnTransform = GetOwner()->GetActorTransform();

	FActorSpawnParameters SpawnParameters;

	SpawnParameters.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AFixedPointPickup* SpawnedActor =
		CurrentWorld->SpawnActor<AFixedPointPickup>(
			Entry.Definition->WorldPickupClass,
			SpawnTransform,
			SpawnParameters);

	if (!IsValid(SpawnedActor))
	{
		UE_LOG(
			LogFixedPointInventory,
			Warning,
			TEXT("DropItem: failed to spawn the dropped pickup"));

		return;
	}

	SpawnedActor->UpdateItemState(Entry);

	if (bDroppingWorldCarry)
	{
		WorldCarryEntry = FFixedPointInventoryEntry{};
	}
	else
	{
		InventoryItems[DroppedIndex] = FFixedPointInventoryEntry{};
	}

	CurrentInventoryIndex = INDEX_NONE;
	CurrentHeldItem = nullptr;
	CanSwapHeldItems = true;

	if (IsValid(OwningFixedPointCharacter))
	{
		OwningFixedPointCharacter->RefreshCarryWeight();
	}

	UE_LOG(
		LogFixedPointInventory,
		Log,
		TEXT("Dropped item: %s | Storage: %s"),
		*GetNameSafe(Entry.Definition.Get()),
		bDroppingWorldCarry
		? TEXT("WorldCarry")
		: TEXT("Inventory"));
}

void UFixedPointInventoryComponent::HandleCarryType(const FFixedPointInventoryEntry Entry, int32 ItemIndex)
{
	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogFixedPointInventory, Warning, TEXT("HandleCarryType() | The owner executing this function does not have authority"))

		return;
	}
	UFixedPointItemDefinition* ItemDefinition = Entry.Definition;

	EFixedPointCarryType CarryType = ItemDefinition->CarryType;
 
	switch (CarryType)
	{
		case EFixedPointCarryType::Pocketable:
		{
			UE_LOG(LogFixedPointInventory, Log, TEXT("Enum Switch Pocketable Code Executing"))

				// if we are not currently holding an item

			if (!CurrentHeldItem)
			{
				
				SetCurrentHeldItem(ItemDefinition);
 
				CanSwapHeldItems = true;

				CurrentInventoryIndex = ItemIndex;
				UE_LOG(LogFixedPointInventory, Log, TEXT("Current inventory index is: %d"), CurrentInventoryIndex)

				//PlayerCharacter->AttachCurrentHeldItem();

			}

			//PlayerCharacter->ApplyWeightChange();
		
			

		
			

			break;

		}


		case EFixedPointCarryType::HandsFull:
		{
			UE_LOG(LogFixedPointInventory, Log, TEXT("Enum Switch HandsFull Code Executing"))

				SetCurrentHeldItem(ItemDefinition);
			CurrentInventoryIndex = ItemIndex;

			CanSwapHeldItems = false;

			
			
			break;

		}



		case EFixedPointCarryType::WorldCarry:
		{
			SetCurrentHeldItem(ItemDefinition);

			CurrentInventoryIndex = INDEX_NONE;
			CanSwapHeldItems = false;

			break;
		}



	}

}

void UFixedPointInventoryComponent::SetCurrentHeldItem(UFixedPointItemDefinition* NewHeldItem)
{

	if (!IsValid(NewHeldItem))
	{
		UE_LOG(LogFixedPointInventory, Warning, TEXT("SetCurrentHeldItem: attempted to set current held item but could not cause the item was invalid"))

		return;
	}


	CurrentHeldItem = NewHeldItem;

	UE_LOG(LogFixedPointInventory, Log, TEXT("SetCurrentHeldItem: Current Held item is now: %s"), *CurrentHeldItem->GetName())




}

void UFixedPointInventoryComponent::SelectInventorySlot(int32 SelectedIndex)
{
	// Make sure this slot actually exists in the current inventory.
	if (!InventoryItems.IsValidIndex(SelectedIndex))
	{
		UE_LOG(LogFixedPointInventory, Warning,
			TEXT("SelectInventorySlot: Invalid SelectedIndex Meaning there is nothing stored in the Array at the index of: %d"),
			SelectedIndex)
			

		return;
	}

	if (!CanChangeInventorySelection())
	{
		return;
	}

	if (CurrentInventoryIndex == SelectedIndex)
	{
		UE_LOG(LogFixedPointInventory, Warning, TEXT("SelectInventorySlot() | Selected Slot is the one were already on "));
		return;
	}

	FFixedPointInventoryEntry ItemEntry = InventoryItems[SelectedIndex];

	UFixedPointItemDefinition* ItemDefinition = ItemEntry.Definition;

	if (!IsValid(ItemDefinition))
	{
		UE_LOG(LogFixedPointInventory, Log, TEXT("SelectInventorySlot() | Item Definition is Invalid"));
		return;
	}

	SetCurrentHeldItem(ItemDefinition);

	CurrentInventoryIndex = SelectedIndex;

	UE_LOG(LogFixedPointInventory, Log, TEXT("SelectInventorySlot: Current Inventory Index is: %d"), CurrentInventoryIndex);

	//PlayerCharacter->AttachCurrentHeldItem();
}

void UFixedPointInventoryComponent::CycleNextInventorySlot()
{
	if (!CanChangeInventorySelection())
	{
		return;
	}

	if (InventoryItems.Num() == 0)
	{
		return;
	}

	// If we're at the last item, cycle back to the first.
	if (CurrentInventoryIndex + 1 >= InventoryItems.Num())
	{
		CurrentInventoryIndex = 0;
	}
	else
	{
		CurrentInventoryIndex += 1;
	}

	FFixedPointInventoryEntry ItemEntry = InventoryItems[CurrentInventoryIndex];

	UFixedPointItemDefinition* ItemDefinition = ItemEntry.Definition;

	if (!IsValid(ItemDefinition))
	{
		return;
	}

	SetCurrentHeldItem(ItemDefinition);

	UE_LOG(LogFixedPointInventory, Log, TEXT("SelectInventorySlot: Current Inventory Index is: %d"), CurrentInventoryIndex);

	//PlayerCharacter->AttachCurrentHeldItem();
}

void UFixedPointInventoryComponent::CyclePreviousInventorySlot()
{
	if (!CanChangeInventorySelection())
	{
		return;
	}

	if (InventoryItems.Num() == 0)
	{
		return;
	}

	// If we're at the first item, cycle to the last.
	if (CurrentInventoryIndex - 1 < 0)
	{
		CurrentInventoryIndex = InventoryItems.Num() - 1;
	}
	else
	{
		CurrentInventoryIndex -= 1;
	}

	FFixedPointInventoryEntry ItemEntry = InventoryItems[CurrentInventoryIndex];

	UFixedPointItemDefinition* ItemDefinition = ItemEntry.Definition;

	if (!IsValid(ItemDefinition))
	{
		return;
	}

	SetCurrentHeldItem(ItemDefinition);

	UE_LOG(LogFixedPointInventory, Log, TEXT("SelectInventorySlot: Current Inventory Index is: %d"), CurrentInventoryIndex);

	//PlayerCharacter->AttachCurrentHeldItem();
}

bool UFixedPointInventoryComponent::CanChangeInventorySelection() const
{
	if (!CanSwapHeldItems)
	{
		UE_LOG(LogFixedPointInventory, Warning, TEXT("CanSelectInventorySlot: Can not swap Held items "))

		return false;
	}

	if (InventoryItems.Num() == 0)
	{
		UE_LOG(LogFixedPointInventory, Warning, TEXT("CanSelectInventorySlot: There is no Items in are inventory "))
		return false;
	}

	return true;

}

float UFixedPointInventoryComponent::CalculateCarryWeight() const
{
	float TotalWeight = 0.0f;

	for (const FFixedPointInventoryEntry& Entry : InventoryItems)
	{
		if (!IsValid(Entry.Definition))
		{
			continue;
		}

		TotalWeight += FMath::Max(0.0f, Entry.Definition->Weight);
	}

	if (IsValid(WorldCarryEntry.Definition))
	{
		TotalWeight +=
			FMath::Max(0.0f, WorldCarryEntry.Definition->Weight);
	}

	return TotalWeight;
}