// Fill out your copyright notice in the Description page of Project Settings.


#include "FixedPointInventoryComponent.h"
#include "Pickups/FixedPointPickup.h"
#include "GameFramework/Actor.h"
#include "../Inventory/FixedPointItemDefinition.h"
#include "Structs/FixedPointInventoryStructs.h"




void UFixedPointInventoryComponent::TryAddItem(AFixedPointPickup* NewItem)
{
	// if this is not being executed on the server return
	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Can not add item because we are executing on a client not the server"));
		return;

	}
	// if the current held item is valid and it is a world carry item then we can not add any item so return
	if (IsValid(CurrentHeldItem) && CurrentHeldItem->CarryType == EFixedPointCarryType::WorldCarry)
	{ 


		UE_LOG(LogTemp, Warning, TEXT("Can not add item because we are holding a world carry item or The Current Held item is not valid"));
		return;
	}

	

	//if this item were trying to add to are inventory is not valid return
	if (!IsValid(NewItem))
	{
		UE_LOG(LogTemp, Warning, TEXT("Can not add item because the item is invalid"));
		return;
	}

	// create a local pointer to the items data asset
	UFixedPointItemDefinition* ItemDefintion = NewItem->GetItemDefinition();

	// if the item definition data asset is not valid return
	if (!IsValid(ItemDefintion))
	{
		UE_LOG(LogTemp, Warning, TEXT("Can not add item to becaused the items definition data asset is invalid"));
		return;

	}

	
	
	// if we cant claim we picked up the item before any one else return cause we didnt pick it up first
	if (!NewItem->TryClaim())
	{
		UE_LOG(LogTemp, Warning, TEXT("Can not add item to inventory someone else already picked it up"));
		return;
	}

	// if inventory items amount is maxed and the item were trying to add is not a world carry
	 
	if (InventoryItems.Num() == MaxInventoryItems && ItemDefintion->CarryType != EFixedPointCarryType::WorldCarry)
	{
		
		
		UE_LOG(LogTemp, Warning, TEXT("Can not add item because inventory is full and the new item were trying to add is not a world carry"));
		return;
		


	}
	
	
	

	
	FFixedPointInventoryEntry Entry;

	Entry.Definition = ItemDefintion;
	Entry.State = NewItem->CaptureItemState();
	

	InventoryItems.Add(Entry);

	for (FFixedPointInventoryEntry& StoredEntry : InventoryItems)
	{
		if (StoredEntry.Definition)
		{
			UE_LOG(LogTemp, Log, TEXT("Item name is: %s"), *StoredEntry.Definition->GetName());
		}
	}

	HandleCarryType(Entry);
	



	NewItem->Destroy();


	

}

void UFixedPointInventoryComponent::HandleCarryType(const FFixedPointInventoryEntry Entry)
{
	UFixedPointItemDefinition* ItemDefintion = Entry.Definition;

	EFixedPointCarryType CarryType = ItemDefintion->CarryType;
 
	switch (CarryType)
	{
		case EFixedPointCarryType::Pocketable:
		{
			if (!CurrentHeldItem)
			{

				SetCurrentHeldItem(ItemDefintion);

				//AttachCurrentHeldItem();

			}


		}


		case EFixedPointCarryType::HandsFull:
		{
			SetCurrentHeldItem(ItemDefintion);

			//AttachCurrentHeldItem();

		}



		case EFixedPointCarryType::WorldCarry:
		{
			// add as the current held item 

		}



	}

}

void UFixedPointInventoryComponent::SetCurrentHeldItem(UFixedPointItemDefinition* NewHeldItem)
{

	if (!IsValid(NewHeldItem))
	{
		return;
	}


	CurrentHeldItem = NewHeldItem;
	


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

