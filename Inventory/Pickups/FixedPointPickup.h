#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../Interfaces/FixedPointInteractableInterface.h"
#include "StructUtils/InstancedStruct.h"
#include "FixedPointPickup.generated.h"

class AFixedPointCharacter;
class UFixedPointItemDefinition;
class UFixedPointInventoryComponent;

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


	//STRUCT

	virtual FInstancedStruct CaptureItemState() const;

	//GETTERS
	UFixedPointItemDefinition* GetItemDefinition()
	{
		return ItemDefinition;
	}

	bool TryClaim();

protected:

	// INVENTORY
	UPROPERTY(BlueprintReadOnly,Category = "Inventory")
	UFixedPointInventoryComponent* InventoryComponent;


	//COMPONENTS
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<UStaticMeshComponent> PickupMesh = nullptr;


	// DATA ASSET
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<UFixedPointItemDefinition> ItemDefinition = nullptr;

	//Store item data 
	// 
	// UStruct overidable

private:

	UPROPERTY()
	bool bTaken = false;


};