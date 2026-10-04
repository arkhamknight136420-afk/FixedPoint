#include "FixedPointMovementComponent.h"

#include "GameFramework/Character.h"

#include "../Characters/FixedPointCharacter.h"


/*
 * Extends Unreal's client-side saved move to record sprint intent for each
 * predicted movement update. This lets the intent be sent to the server and
 * restored when moves are replayed after a correction.
 */
class FSavedMove_FixedPoint : public FSavedMove_Character
{
public:
	using Super = FSavedMove_Character;
	//FSavedMove_Character is a single saved move recording of what the character did at this particular time
	// setting the word super to be another name for "FSavedMove_Character" so we can call funtions on it 
	// through the name super instead of "FSavedMove_Character" this is are saved moves parent class 

	bool bSavedWantsToSprint = false;
	// extending the "FSavedMove_Character" class by creating this child class and adding this variable remember we inherit
	// the parent class "FSavedMove_Character" and all its functions and variables along with the saved sprint bool we made here in this class

	// is called by Unreal on the owning client to populate one saved-move object with the information needed to represent a 
	// particular predicted movement interval.
	virtual void SetMoveFor(
		ACharacter* Character,
		float InDeltaTime,
		const FVector& NewAcceleration,
		FNetworkPredictionData_Client_Character& ClientData) override
		// FNetworkPredictionData_Client_Character is the client side manager for the character’s entire movement prediction process.
		//it manages the client’s entire collection and processing of those movement records.
	{
		Super::SetMoveFor(
			Character,
			InDeltaTime,
			NewAcceleration,
			ClientData);
		// we call the parent version of the function because This lets FSavedMove_Character record all the standard information 
		// it already understands, including timing, acceleration, character state, and ordinary movement flags.Then we add only 
		// the information Unreal doesn’t know about  that being are want to sprint intent
		

		const UFixedPointMovementComponent* MovementComponent = Cast<UFixedPointMovementComponent>(Character->GetCharacterMovement());
		// grab what movement component we are checking through the character we are setting a move for

		if (MovementComponent)
		{
			bSavedWantsToSprint = MovementComponent->WantsToSprint();
			// if that movment component is valid + we want to sprint save that data to FSavedMove_FixedPoint so it knows are intent this move
		}
		
	}

	// Converts the boolean movement states stored in this saved move into a compact
	// 8-bit collection of flags. The base FSavedMove_Character version encodes
	// Unreal's built-in states, such as jump pressed and wants to crouch. Our
	// override preserves those built-in flags and adds sprint intent as Custom_0.
	virtual uint8 GetCompressedFlags() const override
	{
		// Call the parent version first so Unreal can add all its standard
		// movement flags, such as jumping and crouching, to the result.
		// Store that completed 8-bit collection of flags in a local variable.
		uint8 Result = Super::GetCompressedFlags();

		// Check the sprint intent previously recorded for this particular saved move.
		// If it is false, the body is skipped and the sprint bit remains turned off.
		if (bSavedWantsToSprint)
		{
			// Result keeps all the standard flags returned by the parent function,
			// while FLAG_Custom_0 is set to 1 to represent that sprint was requested.
			// so the 8 bit that by default was 0000 0000 is now 0001 0000 for example 
			Result |= FLAG_Custom_0;
		}

		// Give Unreal the completed collection of standard and custom movement flags.
		// Unreal can then include this value in the network data for this move.
		return Result;
	}

	// Resets this saved-move object so Unreal can safely reuse it for a new
	// movement interval without retaining information from its previous use.
	virtual void Clear() override
	{
		// Reset all standard movement information inherited from
		// FSavedMove_Character.
		Super::Clear();

		bSavedWantsToSprint = false;
	}

	// Determines whether this saved move and the newer saved move can be
	// merged into one larger movement interval without losing important state.
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override
	{
		// NewMove is an FSavedMovePtr, which stores the move through Unreal's
	// generic FSavedMove_Character parent type.
	//
	// NewMove.Get() retrieves the ordinary FSavedMove_Character pointer held
	// inside that smart pointer.
	//
	// The actual object will be an FSavedMove_FixedPoint because our custom
	// prediction-data allocator will create that specific saved-move type.
	//
	// static_cast does not create, copy, or change the saved-move object.
	// It changes the compiler's view of the existing pointer from
	// FSavedMove_Character* to FSavedMove_FixedPoint* so we can access our
	// custom bSavedWantsToSprint variable.
	//
	// The const before FSavedMove_FixedPoint means that we can inspect the
	// pointed-to saved move but cannot modify it through NewFixedPointMove.
		const FSavedMove_FixedPoint* NewFixedPointMove = static_cast<const FSavedMove_FixedPoint*>(NewMove.Get());

		// Compare the sprint intent of the current move with the newer move.

		if (bSavedWantsToSprint != NewFixedPointMove->bSavedWantsToSprint)
		{
			// Walking and sprinting moves cannot be merged because they use
			// different maximum-speed behavior.
			return false;
		}
		// Sprint intent matches, but Unreal must still check all its normal
		// requirements before the two moves can be combined.
		return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
	}

		/*PrepMoveFor() runs on the owning client when Unreal is replaying unacknowledged moves after a server correction.
		The execution is:Unreal selects one saved move that needs to be replayed. It calls that move’s PrepMoveFor().Super::PrepMoveFor(Character) 
		restores the standard historical state Unreal understands. We retrieve the local character’s custom movement component. We restore that move’s historical sprint intent:*/
	virtual void PrepMoveFor(ACharacter* Character) override
	{
		// Restore Unreal's standard saved state for this move first.
		Super::PrepMoveFor(Character);
		
		// grab the movement component off this character

		UFixedPointMovementComponent* MovementComponent =Cast<UFixedPointMovementComponent>(Character->GetCharacterMovement());

		if (MovementComponent)
		{
			// Restore this move's historical sprint intent before replaying it.
			MovementComponent->SetWantsToSprint(bSavedWantsToSprint);
		}
	}

	
};

class FNetworkPredictionData_Client_FixedPoint : public FNetworkPredictionData_Client_Character
{
public:

	using Super = FNetworkPredictionData_Client_Character;

	// this is a constructor and as you know when an instance of this class is created this will execute and it requires a movment component to be supplied so whenever
	// we create a FNetworkPredictionData_Client_FixedPoint object u have to give it a movement component
	// the super part is calling the construtor on the parent class because in order for this constructor to be able to execute the parent one must first exist
	// We must construct an FNetworkPredictionData_Client_FixedPoint object,
	// rather than constructing only the parent prediction-data class, because
	// the actual object must be our child type for Unreal to call our overridden
	// AllocateNewMove() function.
	//
	// Every child object also contains its inherited parent-class portion.
	// Unreal's parent prediction-data class requires a character movement
	// component when that parent portion is constructed, so our constructor
	// receives ClientMovement and forwards it to the parent constructor using
	// the initializer list.
	//
	// Our child class does not currently add any member variables that require
	// initialization. Its only addition is the overridden AllocateNewMove()
	// behavior. Therefore, the constructor body is empty; the necessary setup
	// is performed by Super(ClientMovement) before the body is entered.
	explicit FNetworkPredictionData_Client_FixedPoint(const UCharacterMovementComponent& ClientMovement): Super(ClientMovement)
	{
	
	}


	// Unreal calls this when it needs to allocate a new saved-move object
	// and no reusable saved move is available in the FreeMoves pool.
	virtual FSavedMovePtr AllocateNewMove() override
	{
		// Allocate an actual FSavedMove_FixedPoint object in memory.
		//
		// Wrap its address inside Unreal's generic FSavedMovePtr type so
		// the prediction system can manage its lifetime through the normal
		// FSavedMove_Character interface.
		//
		// Although the smart pointer uses the parent type as its interface,
		// the actual object it points to is our FSavedMove_FixedPoint and
		// therefore contains bSavedWantsToSprint and our custom overrides.
		return FSavedMovePtr(new FSavedMove_FixedPoint());
	}
};

FNetworkPredictionData_Client* UFixedPointMovementComponent::GetPredictionData_Client() const
{
	// Client prediction data requires this movement component to belong
	// to a valid Pawn.

	check(PawnOwner != nullptr)

	// Prediction data is created only the first time Unreal requests it.

	if (ClientPredictionData == nullptr)
	{


		UFixedPointMovementComponent* MutableThis = const_cast<UFixedPointMovementComponent*>(this);

		// Create our custom client prediction-data object instead of
		// Unreal's default FNetworkPredictionData_Client_Character.
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_FixedPoint(*this);
	}
	// Return the existing prediction data, whether it was created during
	// this call or during an earlier call.

	return ClientPredictionData;
}

void UFixedPointMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	//That tells Unreal’s parent movement component: “Read your normal flags and restore the built - in movement state you understand, 
	// such as jump and crouch.”

	Super::UpdateFromCompressedFlags(Flags);

	// The single & is bitwise AND, not logical &&. It filters the complete
	// 8-bit Flags value so that only the FLAG_Custom_0 position remains ONE BIT like just 0 or 1.
	// The filtered number will be 0 if that bit was off or a nonzero value
	// (16 for FLAG_Custom_0) if that bit was on. because  each next number is another power of 2 so 1 to the power of 4 is 16
	//
	// != has its normal meaning: "is not equal to." Comparing the filtered
	// number with 0 produces false when the sprint bit was off and true
	// when the sprint bit was on. That Boolean result is then assigned to
	// this movement component's bWantsToSprint.
	// If filtering Flags down to FLAG_Custom_0 produces a nonzero number, the sprint bit was on and bWantsToSprint becomes true;
	// if it produces zero, the bit was off and bWantsToSprint becomes false.
	bWantsToSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;


}

// local function that has nothing to do with networking this sets are intent that we in fact want to sprint
void UFixedPointMovementComponent::SetWantsToSprint(bool bNewWantsToSprint)
{
	bWantsToSprint = bNewWantsToSprint; // updating are sprint intent

	if (bWantsToSprint && CharacterOwner) // if we want to sprint and theres a character that owns this movement component
	{
		CharacterOwner->UnCrouch(); // attempt to uncrouch us
	}
}

// checking whether or not we currently want to sprint by returning are private variable of wants to sprint this is simply a getter function
bool UFixedPointMovementComponent::WantsToSprint() const
{
	return bWantsToSprint;
}

// checks if we are sprinting
bool UFixedPointMovementComponent::IsSprinting() const
{
	return WantsToSprint() && CanSprint();
	// returns true by checking if we intend to sprint and are able to sprint using two different functions
}

bool UFixedPointMovementComponent::CanSprint() const
{
	return IsMovingOnGround() && !IsCrouching();
	// checks if we are crouching or not grounded which are the conditions that allow for sprint 
}

float UFixedPointMovementComponent::GetMaxSpeed() const
{
	// Get the parent's selected speed for walking, crouching, swimming, etc.
	float BaseSpeed = Super::GetMaxSpeed();

	// Use our custom sprint speed when sprinting.
	if (IsSprinting())
	{
		BaseSpeed = SprintSpeed;
	}

	const AFixedPointCharacter* PlayerCharacter =
		Cast<AFixedPointCharacter>(CharacterOwner);

	if (!IsValid(PlayerCharacter))
	{
		return BaseSpeed;
	}

	// Apply weight slowdown to whichever speed was selected.

	return BaseSpeed * PlayerCharacter->GetSpeedReductionMultiplier();
}


