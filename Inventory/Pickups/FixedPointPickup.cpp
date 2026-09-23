#include "FixedPointPickup.h"

#include "../FixedPointItemDefinition.h"
#include "../FixedPointInventoryComponent.h"
#include "../../Player/Characters/FixedPointCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

AFixedPointPickup::AFixedPointPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	SetReplicates(true);
	SetReplicateMovement(true);

	PickupMesh =
		CreateDefaultSubobject<UStaticMeshComponent>(
			TEXT("PickupMesh"));

	SetRootComponent(PickupMesh);

	PickupMesh->SetCollisionProfileName(TEXT("Pickup"));
	PickupMesh->SetGenerateOverlapEvents(true);
}

void AFixedPointPickup::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFixedPointPickup, CarryState);
}

bool AFixedPointPickup::CanInteract_Implementation(
	AFixedPointCharacter* InteractingCharacter) const
{
	const UFixedPointInventoryComponent* Inventory =
		IsValid(InteractingCharacter)
		? InteractingCharacter->GetInventoryComponent()
		: nullptr;

	return IsAvailableInWorld() &&
		IsValid(Inventory) &&
		Inventory->CanAcceptPickup(this);
}

void AFixedPointPickup::Interact_Implementation(
	AFixedPointCharacter* InteractingCharacter)
{
	if (!HasAuthority() ||
		!IsValid(InteractingCharacter))
	{
		return;
	}

	if (UFixedPointInventoryComponent* Inventory =
		InteractingCharacter->GetInventoryComponent())
	{
		Inventory->TryAddPickup(this);
	}
}

void AFixedPointPickup::Use_Implementation(
	AFixedPointCharacter* UsingCharacter)
{
	// Each item class overrides this with its own behavior.
}

AFixedPointCharacter* AFixedPointPickup::GetCarrier() const
{
	return Cast<AFixedPointCharacter>(GetOwner());
}

bool AFixedPointPickup::IsEquippedOrWorldCarried() const
{
	return CarryState == EFixedPointPickupState::Equipped ||
		CarryState == EFixedPointPickupState::WorldCarried;
}

float AFixedPointPickup::GetMeshHalfHeight() const
{
	return IsValid(PickupMesh)
		? FMath::Max(PickupMesh->Bounds.BoxExtent.Z, 10.f)
		: 10.f;
}

void AFixedPointPickup::SetCarriedBy(
	AFixedPointCharacter* Character,
	EFixedPointPickupState NewState)
{
	if (!HasAuthority() ||
		!IsValid(Character) ||
		NewState == EFixedPointPickupState::InWorld)
	{
		return;
	}

	if (GetCarrier() == Character &&
		CarryState == NewState)
	{
		return;
	}

	PickupMesh->SetSimulatePhysics(false);

	SetOwner(Character);

	AttachToComponent(
		Character->GetCapsuleComponent(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	SetActorRelativeTransform(
		NewState == EFixedPointPickupState::WorldCarried
		? WorldCarryRelativeTransform
		: HeldRelativeTransform);

	CarryState = NewState;
	RefreshCarryPresentation();
	ForceNetUpdate();
}

void AFixedPointPickup::DropAt(
	const FVector& Location,
	const FRotator& Rotation)
{
	if (!HasAuthority() ||
		(!IsEquippedOrWorldCarried() &&
			CarryState != EFixedPointPickupState::Stored))
	{
		return;
	}

	DetachFromActor(
		FDetachmentTransformRules::KeepWorldTransform);

	SetOwner(nullptr);

	SetActorLocationAndRotation(
		Location,
		Rotation,
		false,
		nullptr,
		ETeleportType::TeleportPhysics);

	CarryState = EFixedPointPickupState::InWorld;

	RefreshCarryPresentation();
	ForceNetUpdate();
}

void AFixedPointPickup::OnRep_CarryState()
{
	RefreshCarryPresentation();
}

void AFixedPointPickup::RefreshCarryPresentation()
{
	const bool bInWorld =
		CarryState == EFixedPointPickupState::InWorld;

	SetActorEnableCollision(bInWorld);

	SetActorHiddenInGame(
		CarryState == EFixedPointPickupState::Stored);

	if (!bInWorld)
	{
		PickupMesh->SetSimulatePhysics(false);
	}
}