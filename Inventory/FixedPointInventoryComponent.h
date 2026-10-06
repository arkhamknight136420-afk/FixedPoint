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
	

	//NETWORKING

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Bluepure, Category = "Inventory")
	FFixedPointInventorySummary GetConfirmedInventorySummary() const
	{
		return ConfirmedInventorySummary;
	}

	// INVENTORY

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
	

	//WEIGHT

	float CalculateCarryWeight() const;



protected:

	//LIFECYCLE

	UFixedPointInventoryComponent();

	virtual void BeginPlay() override;
	
	//NETWORKING


	//INVENTORY

	

	void HandleCarryType(
		const FFixedPointInventoryEntry Entry,
		int32 ItemIndex);


	UFUNCTION(BlueprintCallable)
	bool CanDropItem();

	//Simply Sets the variable of current held item
	void SetCurrentHeldItem(UFixedPointItemDefinition* NewHeldItem);

	UPROPERTY()
	FFixedPointInventoryEntry WorldCarryEntry;

	UPROPERTY()
	TArray <FFixedPointInventoryEntry> InventoryItems;

	 // DO NOT SET TO LESS THEN ONE
	 UPROPERTY()
	int MaxInventoryItems = 3;


	

private:

	//LIFECYCLE


	//NETWORKING
	
	// ReplicatedUsing marks this property for replication with a notification callback.
	// OnRep_ConfirmedInventorySummary is the name of that callback function.
	// Unreal calls it on the receiving client when replication changes this property.
	/*
	 The Way this works is as follows:
	
	The server changes ConfirmedInventorySummary, either by assigning a new struct or changing one of its members.
	Unreal’s replication system sends the updated property to the owning client.
	Unreal updates that client’s ConfirmedInventorySummary.
	Unreal calls OnRep_ConfirmedInventorySummary() locally on that client.

	so marking the uproperty with "ReplicatedUsing" tells unreal to update the clients version of this property
	and the function we define with = OnRep_ConfirmedInventorySummary is called after the changes to the clients property are made
	*/
	UPROPERTY(ReplicatedUsing = OnRep_ConfirmedInventorySummary)
	FFixedPointInventorySummary ConfirmedInventorySummary;

	UFUNCTION()
	OnRep_ConfirmedInventorySummary();

	//INVENTORY
	bool CanChangeInventorySelection() const;

	void PublishInventorySummary();

	void LogInventorySummary() const;

	void CycleInventorySlot(int32 Direction);

	UPROPERTY()
	int CurrentInventoryIndex = INDEX_NONE;

	UPROPERTY()
	bool CanSwapHeldItems = true;

	UPROPERTY()
	UFixedPointItemDefinition* CurrentHeldItem = nullptr;

	//CHARACTER

	AFixedPointCharacter* OwningFixedPointCharacter = nullptr;


	

		
};
