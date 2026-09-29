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

void UFixedPointInventoryComponent::HandleCarryType(const FFixedPointInventoryEntry Entry)
{
	UFixedPointItemDefinition* ItemDefinition = Entry.Definition;

	EFixedPointCarryType CarryType = ItemDefinition->CarryType;
 
	switch (CarryType)
	{
		case EFixedPointCarryType::Pocketable:
		{
			UE_LOG(LogFixedPointInventory, Log, TEXT("Enum Switch Pocketable Code Executing"))

			if (!CurrentHeldItem)
			{
				
				SetCurrentHeldItem(ItemDefinition);

				CanSwapHeldItems = true;

				//AttachCurrentHeldItem();

			}

		
			//Pick up Pocketable: yes
			//Pick up HandsFull: yes
			//Pick up WorldCarry: yes

		
			

			break;

		}


		case EFixedPointCarryType::HandsFull:
		{
			UE_LOG(LogFixedPointInventory, Log, TEXT("Enum Switch HandsFull Code Executing"))
			//Pick up Pocketable: yes
			//Pick up HandsFull: no
			//Pick up WorldCarry: no

			
			SetCurrentHeldItem(ItemDefinition);

			CanSwapHeldItems = false;
			//AttachCurrentHeldItem();

			
			
			break;

		}



		case EFixedPointCarryType::WorldCarry:
		{
			UE_LOG(LogFixedPointInventory, Log, TEXT("Enum Switch WorldCarry Code Executing"))

			SetCurrentHeldItem(ItemDefinition);

			//Pick up Pocketable: no
			//Pick up HandsFull: no
			//Pick up WorldCarry: no
			
			//AttachCurrentHeldItem();
			//BlockAllInteraction(); POSSIBLY NOT GUARANTEED
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

void UFixedPointInventoryComponent::AttachCurrentHeldItem()
{
	if (!IsValid(CurrentHeldItem))
	{
		return;
	}

	//spawn actor 
	// attach to hand
	//set transform
	//etc etc
}

void UFixedPointInventoryComponent::SelectInventorySlot(int SelectedIndex)
{
	if (SelectedIndex < 0 || SelectedIndex > 2)
	{
		UE_LOG(LogFixedPointInventory, Warning, TEXT("Passed in Inventory index is less then zero or greater then 2"))
		return;
	}

	if (!CanSelectInventorySlot())
	{
		return;
	}


	FFixedPointInventoryEntry ItemEntry = InventoryItems[SelectedIndex];

	UFixedPointItemDefinition* ItemOneDefinition = ItemEntry.Definition;

	SetCurrentHeldItem(ItemOneDefinition);

	// set current held item
	// AttachCurrentHeldItem()
}

bool UFixedPointInventoryComponent::CanSelectInventorySlot() const
{
	if (!CanSwapHeldItems)
	{
		UE_LOG(LogFixedPointInventory, Warning, TEXT("CanSelectInventorySlot: Can not swap Held items "))

		return false;
	}

	return true;

}
