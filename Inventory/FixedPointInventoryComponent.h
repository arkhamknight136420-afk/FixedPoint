#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FixedPointInventoryComponent.generated.h"

class AFixedPointCharacter;
class AFixedPointPickup;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FIXEDPOINT_API UFixedPointInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    static constexpr int32 SlotCount = 3;

    UFixedPointInventoryComponent();

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    AFixedPointPickup* GetItemInSlot(int32 Index) const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    int32 GetSelectedSlot() const
    {
        return SelectedSlot;
    }

    UFUNCTION(BlueprintPure, Category = "Inventory")
    AFixedPointPickup* GetWorldCarriedItem() const
    {
        return WorldCarriedItem.Get();
    }

    UFUNCTION(BlueprintPure, Category = "Inventory")
    float GetTotalWeightPounds() const
    {
        return TotalWeightPounds;
    }

    // State changes occur on the server.
    bool CanAcceptPickup(const AFixedPointPickup* Pickup) const;
    bool TryAddPickup(AFixedPointPickup* Pickup);
    void SelectSlot(int32 Index);
    void CycleSlot(int32 Direction);
    void UseEquippedItem();
    void DropHeldItem();

    UFUNCTION(BlueprintCallable, Category = "Inventory|Lifecycle")
    void HandleOwnerDeath();

    UFUNCTION(BlueprintCallable, Category = "Inventory|Lifecycle")
    void ClearForLoopReset();

private:
    UFUNCTION()
    void OnRep_InventoryState();

    UFUNCTION()
    void OnRep_TotalWeightPounds();

    bool IsServerOwner() const;
    int32 FindFreeSlot() const;
    bool HasHandsFullItem() const;

    AFixedPointCharacter* GetCharacterOwner() const;

    void RefreshEquippedItems();
    void RefreshTotalWeightPounds();
    void DropPickup(AFixedPointPickup* Pickup);

    UPROPERTY(ReplicatedUsing = OnRep_InventoryState)
    TArray<TObjectPtr<AFixedPointPickup>> Slots;

    // INDEX_NONE means empty hands.
    UPROPERTY(ReplicatedUsing = OnRep_InventoryState)
    int32 SelectedSlot = INDEX_NONE;

    // Held separately from the three slots.
    UPROPERTY(ReplicatedUsing = OnRep_InventoryState)
    TObjectPtr<AFixedPointPickup> WorldCarriedItem = nullptr;

    UPROPERTY(ReplicatedUsing = OnRep_TotalWeightPounds)
    float TotalWeightPounds = 0.0f;
};