#include "FixedPointPickup.h"
#include "../../Inventory/FixedPointInventoryComponent.h"
#include "../../Player/Characters/FixedPointCharacter.h"
#include "../FixedPointItemDefinition.h"
#include "../Structs/FixedPointInventoryStructs.h"

DEFINE_LOG_CATEGORY(LogFixedPointPickup);


// LIFE CYCLE

AFixedPointPickup::AFixedPointPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(
		TEXT("PickupMesh"));

	SetRootComponent(PickupMesh);

	PickupMesh->SetCollisionProfileName(TEXT("Pickup"));
	PickupMesh->SetGenerateOverlapEvents(true);
}

//INTERFACE
bool AFixedPointPickup::CanInteract_Implementation(AFixedPointCharacter* InteractingCharacter) const
{
	// if the character that interacted with this object is not valid return false
	if (!IsValid(InteractingCharacter))
	{
		UE_LOG(LogFixedPointPickup, Warning, TEXT("CanInteract_Implementation: The interacting character is not valid"));
		return false;
	}

	// if the object was already picked up by some one else
	if (bTaken)
	{
		UE_LOG(LogFixedPointPickup, Warning, TEXT("CanInteract_Implementation: The Object were trying to pick up was already taken by something or some one else"));
		return false;
	}



	// if the inventory component on the character that interacted with this object is not valid return false

	if (!InteractingCharacter->GetInventoryComponent())
	{
		UE_LOG(LogFixedPointPickup, Warning, TEXT("CanInteract_Implementation: we could not retrieve the inventory component on the interacting character"));
		return false;
	}

	return true;

}


void AFixedPointPickup::Interact_Implementation(AFixedPointCharacter* InteractingCharacter)
{
	// if this interaction is not being executed on the server and or the objects already been picked up return

	if (!HasAuthority())
	{
		UE_LOG(LogFixedPointPickup, Warning,
			TEXT("Interact rejected: this pickup is not running on the server"));
		return;
	}

	if (bTaken)
	{
		UE_LOG(LogFixedPointPickup, Warning,
			TEXT("Interact rejected: this pickup was already taken"));
		return;
	}

	InventoryComponent = InteractingCharacter->GetInventoryComponent();

	UE_LOG(LogFixedPointPickup, Log, TEXT("Interact_Implementation: Calling TryAddItem() "));
	
	InventoryComponent->TryAddItem(this);



	


}

bool AFixedPointPickup::TryClaim()
{
	if (!HasAuthority() || bTaken)
	{
		return false;
	}
	UE_LOG(LogFixedPointPickup, Log, TEXT("TryClaim: Setting bTaken to true claiming this item"));

	bTaken = true;
	return true;
}

  //STRUCTS
FInstancedStruct AFixedPointPickup::CaptureItemState() const
{
	return FInstancedStruct{};
}

void AFixedPointPickup::UpdateItemState(FFixedPointInventoryEntry Entry)
{
	UE_LOG(LogFixedPointPickup, Warning, TEXT("UpdateItemState() | Base class is not meant to be used each class must implement its own definition"));

	return;
}


void AFixedPointPickup::PickupPrimaryFunction()
{
	UE_LOG(LogFixedPointPickup, Warning, TEXT("PickupPrimaryFunction() | Base Class executed"));
}

void AFixedPointPickup::PickupSecondaryFunction()
{
	UE_LOG(LogFixedPointPickup, Warning, TEXT("PickupSecondaryFunction() | Base Class executed"));
}