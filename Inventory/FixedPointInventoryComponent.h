// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs/FixedPointInventoryStructs.h"
#include "FixedPointItemDefinition.h"
#include "FixedPointInventoryComponent.generated.h" 



class AFixedPointPickup;

DECLARE_LOG_CATEGORY_EXTERN(LogFixedPointInventory, Log, All);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIXEDPOINT_API UFixedPointInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	



	UFUNCTION(BlueprintCallable)
	void TryAddItem(AFixedPointPickup* NewItem);

	UFUNCTION(Server, Reliable)
	void ServerSelectInventorySlot(int32 SelectedIndex);

	UFUNCTION(BlueprintCallable)
	void CycleNextInventorySlot();

	UFUNCTION(BlueprintCallable)
	void CyclePreviousInventorySlot();

	

	

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
	 // DO NOT SET TO LESS THEN ONE
	 UPROPERTY()
	int MaxInventoryItems = 3;

	UPROPERTY()
	int MaxWorldCarryItems =  1;


	

private:


	bool CanChangeInventorySelection() const;

	UPROPERTY()
	int CurrentInventoryIndex = 9999;

	UPROPERTY()
	bool CanSwapHeldItems = true;

	UPROPERTY()
	UFixedPointItemDefinition* CurrentHeldItem = nullptr;


	

		
};
