// Fill out your copyright notice in the Description page of Project Settings.


#include "FixedPointInventoryComponent.h"
#include "Pickups/FixedPointPickup.h"
#include "GameFramework/Actor.h"
#include "../Inventory/FixedPointItemDefinition.h"
#include "Structs/FixedPointInventoryStructs.h"

DEFINE_LOG_CATEGORY(LogFixedPointInventory);

void UFixedPointInventoryComponent::TryAddItem(AFixedPointPickup* NewItem)
{
	// if this is not being executed on the server return
	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogFixedPointInventory, Warning, TEXT("Can not add item because we are executing on a client not the server"));
		return;

	}

	//if this item were trying to add to are inventory is not valid return
	if (!IsValid(NewItem))
	{
		UE_LOG(LogFixedPointInventory, Warning, TEXT("Can not add item because the item is invalid"));
		return;
	}

	// create a local pointer to the items data asset
	UFixedPointItemDefinition* ItemDefinition = NewItem->GetItemDefinition();

	// if the item definition data asset is not valid return
	if (!IsValid(ItemDefinition))
	{
		UE_LOG(LogFixedPointInventory, Warning, TEXT("Can not add item to becaused the items definition data asset is invalid"));
		return;

	}
	 

	// First validate NewItem and its definition, then read its carry type.
	const EFixedPointCarryType NewItemCarryType = ItemDefinition->CarryType;

	if (IsValid(CurrentHeldItem))
	{
		if (CurrentHeldItem->CarryType == EFixedPointCarryType::WorldCarry)
		{
			UE_LOG(LogFixedPointInventory, Warning,
				TEXT("Pickup rejected: already holding a WorldCarry item"));
			return;
		}

		if (CurrentHeldItem->CarryType == EFixedPointCarryType::HandsFull &&
			NewItemCarryType != EFixedPointCarryType::Pocketable)
		{
			UE_LOG(LogFixedPointInventory, Warning,
				TEXT("Pickup rejected: holding a HandsFull item; new item is not Pocketable"));
			return;
		}
	}
	

	// if inventory items amount is maxed 
	 
	if (InventoryItems.Num() == MaxInventoryItems)
	{
		// if the item were trying to add is not a world carry
		if (ItemDefinition->CarryType != EFixedPointCarryType::WorldCarry)
		{
			UE_LOG(LogFixedPointInventory, Warning, TEXT("Can not add item because inventory is full and the new item were trying to add is not a world carry"));
			return;
		}



	}

	// if we cant claim we picked up the item before any one else return cause we didnt pick it up first
	if (!NewItem->TryClaim())
	{
		UE_LOG(LogFixedPointInventory, Warning, TEXT("Can not add item to inventory someone else already picked it up"));
		return;
	}
	
	
	

	
	FFixedPointInventoryEntry Entry;

	Entry.Definition = ItemDefinition;
	Entry.State = NewItem->CaptureItemState();
	

	InventoryItems.Add(Entry);

	UE_LOG(LogFixedPointInventory, Log,TEXT("Added item: %s. Inventory count: %d"),*ItemDefinition->GetName(),InventoryItems.Num());

	HandleCarryType(Entry);
	



	NewItem->Destroy();


	

}

bool UFixedPointInventoryComponent::CanDropItem()
{
	// if this is not being executed on the server return false
	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogFixedPointInventory, Warning, TEXT("CanDropItem() | Can not Drop item because we are executing on a client not the server"));
		return false;
	}

	// if the current held item is not valid return false
	if (!IsValid(CurrentHeldItem))
	{
		UE_LOG(LogFixedPointInventory, Warning, TEXT("CanDropItem() | Can not Drop item because Current Held Item Is not valid"));
		return false;
	}
	// if the world pick up class on the current held item is not valid
	if (!IsValid(CurrentHeldItem->WorldPickupClass))
	{
		UE_LOG(LogFixedPointInventory, Warning, TEXT("CanDropItem() | Can not drop item because the WorldPickupClass is not valid"));
		return false;
	}


	return true;


}

void UFixedPointInventoryComponent::DropItem()
{
	if (!CanDropItem())
	{
		return;
	}

	UWorld* CurrentWorld = GetWorld();

	TSubclassOf <AFixedPointPickup> DroppedActorClass = CurrentHeldItem->WorldPickupClass;

	FTransform SpawnLocation = GetOwner()->GetActorTransform();

	FActorSpawnParameters SpawnParameters;

	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;




	if (!IsValid(CurrentWorld))
	{
		UE_LOG(LogFixedPointInventory, Warning, TEXT("DropItem() | Could not Spawn the item trying to be dropped because the current world were in is not valid"))
	}

	AFixedPointPickup* SpawnedActor = CurrentWorld->SpawnActor <AFixedPointPickup>(DroppedActorClass, SpawnLocation,SpawnParameters);

	if (!IsValid(SpawnedActor))
	{
		UE_LOG(LogFixedPointInventory, Warning, TEXT("DropItem() | Item We Spawned is not valid"))
	}

	UE_LOG(LogFixedPointInventory, Log, TEXT("DropItem() | Item Spawned Succesfully "))

	FFixedPointInventoryEntry Entry = InventoryItems[CurrentInventoryIndex];

	// probably check if entry is valid somehow 


	SpawnedActor->UpdateItemState(Entry);


	InventoryItems[CurrentInventoryIndex] = FFixedPointInventoryEntry{};

	CurrentInventoryIndex = INDEX_NONE;

	CurrentHeldItem = nullptr;


	

}

void UFixedPointInventoryComponent::HandleCarryType(const FFixedPointInventoryEntry Entry)
{
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

				//PlayerCharacter->AttachCurrentHeldItem();

			}

			//PlayerCharacter->ApplyWeightChange();
		
			

		
			

			break;

		}


		case EFixedPointCarryType::HandsFull:
		{
			UE_LOG(LogFixedPointInventory, Log, TEXT("Enum Switch HandsFull Code Executing"))

			SetCurrentHeldItem(ItemDefinition);

			CanSwapHeldItems = false;

			//PlayerCharacter->AttachCurrentHeldItem();

			//PlayerCharacter->ApplyWeightChange();


			
			
			break;

		}



		case EFixedPointCarryType::WorldCarry:
		{
			UE_LOG(LogFixedPointInventory, Log, TEXT("Enum Switch WorldCarry Code Executing"))

			SetCurrentHeldItem(ItemDefinition);

			CanSwapHeldItems = false;

			//PlayerCharacter->AttachCurrentHeldItem();

			//PlayerCharacter->ApplyWeightChange();


			UE_LOG(LogFixedPointInventory, Log, TEXT(" HandleCarryType World Carry: Current Inventory Index is: %d"), CurrentInventoryIndex);

			
		

			

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
