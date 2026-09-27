#include "FixedPointPickup.h"
#include "../../Inventory/FixedPointInventoryComponent.h"
#include "../../Player/Characters/FixedPointCharacter.h"
#include"../FixedPointItemDefinition.h"



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
		return false;
	}

	// if the object was already picked up by some one else
	if (bTaken)
	{
		return false;
	}



	// if the inventory component on the character that interacted with this object is not valid return false

	if (!InteractingCharacter->GetInventoryComponent())
	{
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("Executed can interact default implementation which returns true"));
	return true;

}


void AFixedPointPickup::Interact_Implementation(AFixedPointCharacter* InteractingCharacter)
{
	// if this interaction is not being executed on the server and or the objects already been picked up return

	if (!HasAuthority() || bTaken)
	{
		return;
	}

	InventoryComponent = InteractingCharacter->GetInventoryComponent();

	UE_LOG(LogTemp, Log, TEXT("Executed interact default implementation so something would be interacted with here"));
	
	InventoryComponent->TryAddItem(this);



	


}

bool AFixedPointPickup::TryClaim()
{
	if (!HasAuthority() || bTaken)
	{
		return false;
	}

	bTaken = true;
	return true;
}

  //STRUCTS
FInstancedStruct AFixedPointPickup::CaptureItemState() const
{
	return FInstancedStruct{};
}

