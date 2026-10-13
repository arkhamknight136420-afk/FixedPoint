// Fill out your copyright notice in the Description page of Project Settings.


#include "FixedPointInventoryComponent.h"
#include "Pickups/FixedPointPickup.h"
#include "GameFramework/Actor.h"
#include "../Inventory/FixedPointItemDefinition.h"
#include "Structs/FixedPointInventoryStructs.h"
#include "../Player/Characters/FixedPointCharacter.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogFixedPointInventory);


// LIFECYCLE

UFixedPointInventoryComponent::UFixedPointInventoryComponent()
{
	// This enables component replication by default. Individual properties still need to be registered for replication.
	//Your character already creates the inventory as a default subobject and sets bReplicates = true. 
	// Therefore, this addition completes the relevant component-level setup. 
	// Epic’s documented pattern requires both the owning actor and the component to replicate
	SetIsReplicatedByDefault(true);
}

void UFixedPointInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningFixedPointCharacter = Cast<AFixedPointCharacter>(GetOwner());

	if (!IsValid(OwningFixedPointCharacter))
	{
		UE_LOG(LogFixedPointInventory, Error, TEXT("UFixedPointInventoryComponent::BeginPlay() | Owning Character is not valid"))

		return;
	}

	if (!OwningFixedPointCharacter->HasAuthority())
	{
		UE_LOG(LogFixedPointInventory, Verbose, TEXT("UFixedPointInventoryComponent::BeginPlay() | Owning Character Does not have authority"))
		return;
	}

	// creates three inventory Entires in the array of inventory items all definitions in it are nullptr and the state is empty
	InventoryItems.SetNum(MaxInventoryItems);

	//Makes it so that are Starting inventory slot were on is the first slot
	CurrentInventoryIndex = 0;

	PublishInventorySummary();
}

//NETWORKING

void UFixedPointInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{ //Override Unreal’s function that describes this component’s replicated properties, adding our properties to the supplied list. Properties as in variables

	// This lets the parent register its inherited replicated properties in the same list.
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// this is a macro that will let us register are additional properties that need to be replicated
	// first input is the class declaring the properties the reason why is because Because the macro needs to identify a particular property declaration—and that property belongs to a particular class.
	// These two arguments work together :UFixedPointInventoryComponent, ConfirmedInventorySummary They mean 
	// :The member named ConfirmedInventorySummary belonging to UFixedPointInventoryComponent.
	// A property name is not unique throughout the program.For example, two different classes could both contain a property named Health :
	// next is the property being registered
	// last is the condition controlling which connection receives it.
	// COND_OwnerOnly limits delivery to the actor’s owning connection.ReplicatedUsing supplies the associated notification callback.
	// that condition is responsivle for deciding who recieves this properties data

	DOREPLIFETIME_CONDITION(UFixedPointInventoryComponent, ConfirmedInventorySummary, COND_OwnerOnly);
}

void UFixedPointInventoryComponent::PublishInventorySummary()
{
	AActor* Owner = GetOwner();

	if (!IsValid(Owner))
	{
		UE_LOG(LogFixedPointInventory, Warning, TEXT("PublishInventorySummary() | The Owner is not valid "))

		return;
	}

	if (!Owner->HasAuthority())
	{
		UE_LOG(LogFixedPointInventory, Warning, TEXT("PublishInventorySummary() | The Owner Does Not Have Authority "))

		return;
	}

	//create a new local instance of are inventory summary struct
	FFixedPointInventorySummary NewSummary;

	// begin copying all the current state variables into the summary 

	//creates 3 entries in the slot definition array stored in are local new summary struct, Newly created object-pointer entries start empty.
	NewSummary.SlotDefinitons.SetNum(MaxInventoryItems);

	// Starting at index 0, copy each inventory entry's definition into the
	// matching summary slot. Advance the index after each copy, stopping
	// when we reach the inventory array's size or the maximum slot count.
	for (int32 Index = 0; Index < InventoryItems.Num() && Index < MaxInventoryItems; ++Index)
	{
		NewSummary.SlotDefinitons[Index] = InventoryItems[Index].Definition;
	}

	NewSummary.WorldCarryDefinition = WorldCarryEntry.Definition;
	
	NewSummary.HeldDefinition = CurrentHeldItem;

	NewSummary.SelectedSlotIndex = CurrentInventoryIndex;

	NewSummary.bCanSwapHeldItems = CanSwapHeldItems;

	NewSummary.TotalCarryWeight = CalculateCarryWeight();

	// if the the int32 variable named revision is equal to the max possible 32 bit integer then set the number to one other wise take the variable add one to it
	// and set it equal to that
	NewSummary.Revision = ConfirmedInventorySummary.Revision == MAX_int32 ? 1 : ConfirmedInventorySummary.Revision + 1;

	// MoveTemp enables move assignment, allowing the destination to take ownership
	// of NewSummary's array storage instead of copying its entries.
	// We use it because we no longer need NewSummary's contents afterward.
	ConfirmedInventorySummary = MoveTemp(NewSummary);

	//This asks Unreal to update replication for the owning actor sooner. We call it on the actor because this component replicates as part of that actor.
	Owner->ForceNetUpdate();

	// This prints the server’s stored copy.
	// Publication remains an ordinary server - side operation : build the data, assign it, request replication, and print it.
	LogInventorySummary();
}

void UFixedPointInventoryComponent::OnRep_ConfirmedInventorySummary()
{
	LogInventorySummary();
}

void UFixedPointInventoryComponent::LogInventorySummary() const
{
	const AActor* Owner = GetOwner();

	UE_LOG(
		LogFixedPointInventory,
		Log,
		TEXT("Confirmed inventory | Owner: %s | Authority: %s | Revision: %d | Selected: %d | Held: %s | WorldCarry: %s | Can swap: %s | Weight: %.2f lb"),
		*GetNameSafe(Owner),
		IsValid(Owner) && Owner->HasAuthority()
		? TEXT("Server")
		: TEXT("Client"),
		ConfirmedInventorySummary.Revision,
		ConfirmedInventorySummary.SelectedSlotIndex,
		*GetNameSafe(ConfirmedInventorySummary.HeldDefinition.Get()),
		*GetNameSafe(ConfirmedInventorySummary.WorldCarryDefinition.Get()),
		ConfirmedInventorySummary.bCanSwapHeldItems
		? TEXT("true")
		: TEXT("false"),
		ConfirmedInventorySummary.TotalCarryWeight);

}
//INVENTORY

bool UFixedPointInventoryComponent::TryAddItem(
	AFixedPointPickup* NewItem)
{
	AActor* Owner = GetOwner();

	if (!IsValid(Owner) ||
		!Owner->HasAuthority() ||
		!IsValid(OwningFixedPointCharacter) ||
		!IsValid(NewItem))
	{
		return false;
	}

	UFixedPointItemDefinition* ItemDefinition =
		NewItem->GetItemDefinition();

	if (!IsValid(ItemDefinition))
	{
		return false;
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

		return false;
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

		return false;
	}
	// Where we will store the incoming pickup.
	int32 SlotIndex = INDEX_NONE;

	// if the item is not a world carry execute this code
	if (!bIsWorldCarry)
	{
		// if the new added inventory items carry type is pocketable and the index were on in the inventory is valid 
		// and there is currently no valid item definition there
		if (NewCarryType == EFixedPointCarryType::Pocketable &&
			InventoryItems.IsValidIndex(CurrentInventoryIndex) &&
			!IsValid(InventoryItems[CurrentInventoryIndex].Definition))
		{
			// copy the inventory index were at into the local slot index variable cause this means we wanna add the item here cause theres nothing in this slot
			SlotIndex = CurrentInventoryIndex;
		}

		//search else where only if we havent chosen an inventory slot 
		// Because we initialize it with SlotIndex = INDEX_NONE;.If the selected slot is already occupied, the previous check doesn’t change
		// SlotIndex, so it remains INDEX_NONE.That tells us we still need to find an empty slot.
		if (SlotIndex == INDEX_NONE)
		{
			// whenever the index were currently on in the inventory array is less then the max holdable items execute this code then add one to index
			for (int32 Index = 0; Index < InventoryItems.Num(); ++Index)
			{
				// if the stored item definition in the index were on in the array is not valid that means its an empty slot we can store something in 
				// so set slot index the variable thats deciding where we will store the incoming pickup  to  the index were on in the loop 
				if (!IsValid(InventoryItems[Index].Definition))
				{
					SlotIndex = Index;
					break;
				}
			}
		}

		// SlotIndex == INDEX_NONE: We did not find an empty slot.
		// &&: Both conditions must be true.
		// InventoryItems.Num() >= MaxInventoryItems:
		// The array already contains the maximum allowed number of slots.
		// Num() counts ALL slots, including empty ones.
		if (SlotIndex == INDEX_NONE &&
			InventoryItems.Num() >= MaxInventoryItems)
		{
			UE_LOG(
				LogFixedPointInventory, // Write to our inventory log category.
				Warning, // Display the message as a warning.
				TEXT("Pickup rejected: inventory is full")); // Message to display.

			// Exit TryAddItem() before claiming or storing the pickup.
			return false; 
		}

	}

	// Claim only after checking carry restrictions and capacity.
	if (!NewItem->TryClaim())
	{
		UE_LOG(
			LogFixedPointInventory,
			Warning,
			TEXT("Pickup rejected: item was already claimed"));

		return false;
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

	PublishInventorySummary();

	UE_LOG(
		LogFixedPointInventory,
		Log,
		TEXT("Picked up: %s | Inventory array size: %d | WorldCarry: %s"),
		*GetNameSafe(ItemDefinition),
		InventoryItems.Num(),
		*GetNameSafe(WorldCarryEntry.Definition.Get()));

	NewItem->Destroy();

	return true;
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

	// First, copy the item we are dropping.
	// Keep the original storage unchanged until spawning succeeds.
	FFixedPointInventoryEntry Entry;

	if (bDroppingWorldCarry)
	{
		Entry = WorldCarryEntry;
	}
	else
	{
		Entry = InventoryItems[DroppedIndex];
	}

	// Protect against accessing an empty definition.
	if (!IsValid(Entry.Definition))
	{
		UE_LOG(
			LogFixedPointInventory,
			Error,
			TEXT("DropItem: copied entry has no valid definition"));

		return;
	}

	if (!Entry.Definition->WorldPickupClass)
	{
		UE_LOG(
			LogFixedPointInventory,
			Warning,
			TEXT("DropItem: item has no world pickup class"));

		return;
	}

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

	// Restore the dropped object's saved state.
	SpawnedActor->UpdateItemState(Entry);

	// Spawning succeeded. Now update our storage and held item.
	if (bDroppingWorldCarry)
	{
		WorldCarryEntry = FFixedPointInventoryEntry{};

		UFixedPointItemDefinition* SelectedSlotDefinition = nullptr;

		if (InventoryItems.IsValidIndex(CurrentInventoryIndex))
		{
			SelectedSlotDefinition =
				InventoryItems[CurrentInventoryIndex].Definition;
		}

		SetCurrentHeldItem(SelectedSlotDefinition);
	}
	else
	{
		InventoryItems[DroppedIndex] = FFixedPointInventoryEntry{};

		SetCurrentHeldItem(nullptr);
	}

	CanSwapHeldItems = true;

	if (IsValid(OwningFixedPointCharacter))
	{
		OwningFixedPointCharacter->RefreshCarryWeight();
	}

	PublishInventorySummary();

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

			CanSwapHeldItems = false;

			break;
		}



	}

}

void UFixedPointInventoryComponent::SetCurrentHeldItem(UFixedPointItemDefinition* NewHeldItem)
{


	CurrentHeldItem = NewHeldItem;

	UE_LOG(
		LogFixedPointInventory,
		Log,
		TEXT("SetCurrentHeldItem: Current held item is now: %s"),
		*GetNameSafe(CurrentHeldItem));

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

	SetCurrentHeldItem(ItemDefinition);

	CurrentInventoryIndex = SelectedIndex;

	PublishInventorySummary();

	UE_LOG(LogFixedPointInventory, Log, TEXT("SelectInventorySlot: Current Inventory Index is: %d"), CurrentInventoryIndex);

	//PlayerCharacter->AttachCurrentHeldItem();
}

void UFixedPointInventoryComponent::CycleNextInventorySlot()
{
	CycleInventorySlot(1);
}

void UFixedPointInventoryComponent::CyclePreviousInventorySlot()
{

	CycleInventorySlot(-1);

}

void UFixedPointInventoryComponent::CycleInventorySlot(int32 Direction)
{
	if (!CanChangeInventorySelection())
	{
		UE_LOG(
			LogFixedPointInventory,
			Warning,
			TEXT("CycleInventorySlot: Cannot change inventory selection"));

		return;
	}

	if (Direction != 1 && Direction != -1)
	{
		UE_LOG(
			LogFixedPointInventory,
			Warning,
			TEXT("CycleInventorySlot: Direction must be 1 or -1"));

		return;
	}

	const int32 NumSlots = InventoryItems.Num();

	int32 CandidateIndex;

	if (InventoryItems.IsValidIndex(CurrentInventoryIndex))
	{
		CandidateIndex = CurrentInventoryIndex;
	}
	else
	{
		if (Direction == 1)
		{
			CandidateIndex = NumSlots - 1;
		}
		else
		{
			CandidateIndex = 0;
		}
	}

	CandidateIndex = CandidateIndex + Direction;

	if (CandidateIndex >= NumSlots)
	{
		CandidateIndex = 0;
	}
	else if (CandidateIndex < 0)
	{
		CandidateIndex = NumSlots - 1;
	}

	SelectInventorySlot(CandidateIndex);
}


bool UFixedPointInventoryComponent::CanChangeInventorySelection() const
{
	const AActor* Owner = GetOwner();

	if (!IsValid(Owner) || !Owner->HasAuthority())
	{
		return false;
	}

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


