#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../Interfaces/FixedPointInteractableInterface.h"
#include "FixedPointPickup.generated.h"

class AFixedPointCharacter;
class UFixedPointItemDefinition;

UCLASS()
class FIXEDPOINT_API AFixedPointPickup
	: public AActor,
	public IFixedPointInteractableInterface
{
	GENERATED_BODY()

public:
	// LIFE CYCLE
	AFixedPointPickup();

	//INTERFACE
	virtual void Interact_Implementation(
		AFixedPointCharacter* InteractingCharacter) override;

	virtual bool CanInteract_Implementation(
		AFixedPointCharacter* InteractingCharacter) const override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<UStaticMeshComponent> PickupMesh = nullptr;


	// DATA ASSET
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<UFixedPointItemDefinition> ItemDefinition = nullptr;
};