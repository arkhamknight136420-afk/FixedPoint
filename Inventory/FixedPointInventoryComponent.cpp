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
		return;
	}

	//if this item were trying to add to are inventory is not valid return
	if (!IsValid(NewItem))
	{
		return;
	}

	// create a local pointer to the items data asset
	UFixedPointItemDefinition* ItemDefintion = NewItem->GetItemDefinition();

	// if the item definition data asset is not valid return
	if (!IsValid(ItemDefintion))
	{
		return;
	}


	//create NewItemSavedvalues

	

	FFixedPointInventoryEntry Entry;

	Entry.Definition = ItemDefintion;
	Entry.State = NewItem->CaptureItemState();

	// if someone already grabbed this item  then return as another person should not be able to add it to there inventory
	if (!NewItem->TryClaim())
	{
		UE_LOG(LogTemp, Log, TEXT("Can not add item to inventory someone else already picked it up"));
		return;
	}

	InventoryItems.Add(Entry);

	for (FFixedPointInventoryEntry& StoredEntry : InventoryItems)
	{
		if (StoredEntry.Definition)
		{
			UE_LOG(LogTemp, Log, TEXT("Item name is: %s"),*StoredEntry.Definition->GetName());
		}
	}



	NewItem->Destroy();


}

