#include "FixedPointPickup.h"


// LIFE CYCLE

AFixedPointPickup::AFixedPointPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(
		TEXT("PickupMesh"));

	SetRootComponent(PickupMesh);

	PickupMesh->SetCollisionProfileName(TEXT("Pickup"));
	PickupMesh->SetGenerateOverlapEvents(true);
}

//INTERFACE
bool AFixedPointPickup::CanInteract_Implementation(AFixedPointCharacter* InteractingCharacter) const
{
	UE_LOG(LogTemp, Log, TEXT("Executed can interact default implementation which returns true"));

	return true;
}


void AFixedPointPickup::Interact_Implementation(AFixedPointCharacter* InteractingCharacter)
{
	UE_LOG(LogTemp, Log, TEXT("Executed interact default implementation so something would be interacted with here"));

	// depending on enum type do a certain thing to the player 
}