// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs/FixedPointInventoryStructs.h"
#include "FixedPointItemDefinition.h"
#include "FixedPointInventoryComponent.generated.h" 



class AFixedPointPickup;
class AFixedPointCharacter;

DECLARE_LOG_CATEGORY_EXTERN(LogFixedPointInventory, Log, All);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIXEDPOINT_API UFixedPointInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	//LIFECYCLE
	UFixedPointInventoryComponent();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void TryAddItem(AFixedPointPickup* NewItem);

	

	UFUNCTION(BlueprintCallable)
	void DropItem();

	UFUNCTION(BlueprintCallable)
	void SelectInventorySlot(int32 SelectedIndex);

	UFUNCTION(BlueprintCallable)
	void CycleNextInventorySlot();

	UFUNCTION(BlueprintCallable)
	void CyclePreviousInventorySlot();

	
	UFixedPointItemDefinition* GetCurrentHeldItem() const
	{
		return CurrentHeldItem;
	}
	

protected:
	
	

	// void RemoveItem

	// void Has item

	TArray <FFixedPointInventoryEntry> InventoryItems;

	

	
	void HandleCarryType(
		const FFixedPointInventoryEntry Entry,
		int32 ItemIndex);

	
	UFUNCTION(BlueprintCallable)
	bool CanDropItem();
	

	

	//Simply Sets the variable of current held item
	void SetCurrentHeldItem(UFixedPointItemDefinition* NewHeldItem);

	


	
	 // DO NOT SET TO LESS THEN ONE
	 UPROPERTY()
	int MaxInventoryItems = 3;

	UPROPERTY()
	int MaxWorldCarryItems =  1;


	

private:


	bool CanChangeInventorySelection() const;

	UPROPERTY()
	int CurrentInventoryIndex = INDEX_NONE;

	UPROPERTY()
	bool CanSwapHeldItems = true;

	UPROPERTY()
	UFixedPointItemDefinition* CurrentHeldItem = nullptr;

	AFixedPointCharacter* OwningFixedPointCharacter = nullptr;


	

		
};
