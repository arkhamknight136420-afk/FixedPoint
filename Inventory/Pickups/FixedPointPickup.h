#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../Interfaces/FixedPointInteractableInterface.h"
#include "FixedPointPickup.generated.h"

class AFixedPointCharacter;
class UFixedPointItemDefinition;
class UStaticMeshComponent;

UENUM(BlueprintType)
enum class EFixedPointPickupState : uint8
{
	InWorld,
	Stored,
	Equipped,
	WorldCarried
};

UCLASS()
class FIXEDPOINT_API AFixedPointPickup
	: public AActor,
	public IFixedPointInteractableInterface
{
	GENERATED_BODY()

public:
	AFixedPointPickup();

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Interact_Implementation(
		AFixedPointCharacter* InteractingCharacter) override;

	virtual bool CanInteract_Implementation(
		AFixedPointCharacter* InteractingCharacter) const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
	void Use(AFixedPointCharacter* UsingCharacter);

	virtual void Use_Implementation(
		AFixedPointCharacter* UsingCharacter);

	UFUNCTION(BlueprintPure, Category = "Item")
	UFixedPointItemDefinition* GetItemDefinition() const
	{
		return ItemDefinition.Get();
	}

	UFUNCTION(BlueprintPure, Category = "Item")
	AFixedPointCharacter* GetCarrier() const;

	UFUNCTION(BlueprintPure, Category = "Item")
	bool IsAvailableInWorld() const
	{
		return CarryState == EFixedPointPickupState::InWorld;
	}

	bool IsEquippedOrWorldCarried() const;
	float GetMeshHalfHeight() const;

	// Called by the inventory on the server.
	void SetCarriedBy(
		AFixedPointCharacter* Character,
		EFixedPointPickupState NewState);

	void DropAt(
		const FVector& Location,
		const FRotator& Rotation);

protected:
	virtual void RefreshCarryPresentation();

	UFUNCTION()
	void OnRep_CarryState();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<UStaticMeshComponent> PickupMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<UFixedPointItemDefinition> ItemDefinition = nullptr;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Pickup|Carrying")
	FTransform HeldRelativeTransform =
		FTransform(FVector(45.f, 25.f, 25.f));

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Pickup|Carrying")
	FTransform WorldCarryRelativeTransform =
		FTransform(FVector(70.f, 0.f, 0.f));

	UPROPERTY(
		ReplicatedUsing = OnRep_CarryState,
		BlueprintReadOnly,
		Category = "Pickup|Carrying")
	EFixedPointPickupState CarryState =
		EFixedPointPickupState::InWorld;
};