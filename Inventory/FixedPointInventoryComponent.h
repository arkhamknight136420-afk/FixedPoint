// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs/FixedPointInventoryStructs.h"
#include "FixedPointInventoryComponent.generated.h" 
#include "FixedPointItemDefinition.h"

class AFixedPointPickup;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIXEDPOINT_API UFixedPointInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	



	UFUNCTION(BlueprintCallable)
	void TryAddItem(AFixedPointPickup* NewItem);



protected:
	
	

	// void RemoveItem

	// void Has item

	TArray <FFixedPointInventoryEntry> InventoryItems;

	

	
	void HandleCarryType(const FFixedPointInventoryEntry Entry);

	

	void AttachCurrentHeldItem();

	//bool CanSwapHeldItems()

	//Simply Sets the variable of current held item
	void SetCurrentHeldItem(UFixedPointItemDefinition* NewHeldItem);

	


	UFixedPointItemDefinition* GetCurrentHeldItem() const
	{
		return CurrentHeldItem;
	}

	 UPROPERTY()
	int MaxInventoryItems = 3;
	

private:

	UPROPERTY()
	int MaxinventorySpace = 3;

	UPROPERTY()
	UFixedPointItemDefinition* CurrentHeldItem = nullptr;


	

		
};
