#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FixedPointMovementComponent.generated.h"
UCLASS()
class FIXEDPOINT_API UFixedPointMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:

	void SetWantsToSprint(bool bNewWantsToSprint);

	bool WantsToSprint() const;

	bool IsSprinting() const; 

	virtual float GetMaxSpeed() const override;

	// Reads the compressed movement flags for a move and reconstructs
	// our movement component's sprint intent from FLAG_Custom_0.
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;


protected:

	virtual bool CanSprint() const;


	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Movement|Sprint",
		meta = (ClampMin = "0.0", UIMin = "0.0"))
	float SprintSpeed = 700.0f;

	// Maximum fraction of movement speed lost to weight.
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Movement|Weight",
		meta = (
			ClampMin = "0.0",
			ClampMax = "0.9",
			UIMin = "0.0",
			UIMax = "0.9"))
	float MaxWeightSlowdownFraction = 0.35f;

	// At this weight, the slowdown reaches half of its maximum.
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Movement|Weight",
		meta = (ClampMin = "1.0", UIMin = "1.0"))
	float WeightHalfSlowdownPounds = 40.0f;

private:

	bool bWantsToSprint = false;

	

	



};