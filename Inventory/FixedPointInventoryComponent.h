// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs/FixedPointInventoryStructs.h"
#include "FixedPointInventoryComponent.generated.h"

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

	AFixedPointPickup* CurrentHeldItem = nullptr;

	

	// bool CanPickUpItems()

	//bool CanSwapHeldItems()



private:

	int MaxinventorySpace = 3;
	

		
};
