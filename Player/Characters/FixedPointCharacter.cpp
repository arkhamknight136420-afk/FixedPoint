// Fill out your copyright notice in the Description page of Project Settings.

#include "FixedPointCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "../MovementComponents/FixedPointMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../../Interfaces/FixedPointInteractableInterface.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "../../Inventory/FixedPointInventoryComponent.h"

DEFINE_LOG_CATEGORY(LogFixedPointCharacter);

// LIFE CYCLE

AFixedPointCharacter::AFixedPointCharacter(
	const FObjectInitializer& ObjectInitializer)
	: Super(
		ObjectInitializer.SetDefaultSubobjectClass<UFixedPointMovementComponent>(
			ACharacter::CharacterMovementComponentName))
{
	bReplicates = true;

	InitializeMovementComponent();

	InitializeCameraComponent();

	InitializeInteractionCapsuleComponent();

	InitializeInventoryComponent();

	
}

void AFixedPointCharacter::BeginPlay()
{
	Super::BeginPlay();

	BindInteractionCapsuleEvents();
}

// MOVEMENT

void AFixedPointCharacter::InitializeMovementComponent()
{

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();

	bUseControllerRotationPitch = false;

	bUseControllerRotationYaw = true;

	bUseControllerRotationRoll = false;

	MovementComponent->bOrientRotationToMovement = false;

	MovementComponent->GetNavAgentPropertiesRef().bCanCrouch = true;
}

void AFixedPointCharacter::Move(const FVector2D& MovementInput)
{
	if (!Controller)
	{
		return;
	}

	// Ignore control pitch and roll so looking up or down never makes forward
	// movement push the character into the floor or into the air.
	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);

	const FVector ForwardDirection =
		FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	const FVector RightDirection =
		FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementInput.Y);
	AddMovementInput(RightDirection, MovementInput.X);
}

void AFixedPointCharacter::SetSprintRequested(const bool bRequested)
{
	UFixedPointMovementComponent* MovementComponent =
		CastChecked<UFixedPointMovementComponent>(GetCharacterMovement());

	MovementComponent->SetWantsToSprint(bRequested);
}

bool AFixedPointCharacter::IsSprintRequested() const
{
	const UFixedPointMovementComponent* MovementComponent =
		CastChecked<UFixedPointMovementComponent>(GetCharacterMovement());

	return MovementComponent->WantsToSprint();
}

void AFixedPointCharacter::StartJump()
{
	Jump();
}

void AFixedPointCharacter::StopJump()
{
	StopJumping();
}

void AFixedPointCharacter::StartCrouch()
{
	Crouch();
}

void AFixedPointCharacter::ToggleCrouch()
{
	if (IsCrouched())
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AFixedPointCharacter::StopCrouch()
{
	UnCrouch();
}

void AFixedPointCharacter::SelectInventorySlot1()
{
	if (IsValid(InventoryComponent))
	{
		ServerSelectInventorySlot(0);
	}
}

void AFixedPointCharacter::SelectInventorySlot2()
{
	if (IsValid(InventoryComponent))
	{
		ServerSelectInventorySlot(1);
	}
}

void AFixedPointCharacter::SelectInventorySlot3()
{
	if (IsValid(InventoryComponent))
	{
		ServerSelectInventorySlot(2);
	}
}

void AFixedPointCharacter::ServerSelectInventorySlot_Implementation(int32 SelectedIndex)
{
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->SelectInventorySlot(SelectedIndex);
	}
}


void AFixedPointCharacter::SelectNextInventorySlot()
{
	if (IsValid(InventoryComponent))
	{
		ServerCycleNextInventorySlot();
	}
}

void AFixedPointCharacter::SelectPreviousInventorySlot()
{
	if (IsValid(InventoryComponent))
	{
		ServerCyclePreviousInventorySlot();
	}
}
void AFixedPointCharacter::ServerCycleNextInventorySlot_Implementation()
{
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->CycleNextInventorySlot();
	}
}

void AFixedPointCharacter::ServerCyclePreviousInventorySlot_Implementation()
{
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->CyclePreviousInventorySlot();
	}
}

void AFixedPointCharacter::DropItemStarted()
{
	if (IsValid(InventoryComponent))
	{
		ServerDropItem();
	}
}

void AFixedPointCharacter::ServerDropItem_Implementation()
{
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->DropItem();
	}
}

void AFixedPointCharacter::StartUseItemPrimary()
{

}

void AFixedPointCharacter::StopUseItemPrimary()
{

}

void AFixedPointCharacter::StartUseItemSecondary()
{

}

void AFixedPointCharacter::StopUseItemSecondary()
{

}





// INTERACTION

void AFixedPointCharacter::InitializeInteractionCapsuleComponent()
{
	InteractionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("InteractionCapsule"));

	InteractionCapsule->SetupAttachment(PlayerCamera);

	InteractionCapsule->SetRelativeRotation(FRotator(90.f, 0, 0));

	InteractionCapsule->SetCapsuleHalfHeight(130.f);

	InteractionCapsule->SetCapsuleRadius(30.f);

	InteractionCapsule->SetRelativeLocation(FVector(110.f, 0, 0));

	InteractionCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	InteractionCapsule->SetCollisionObjectType(ECC_WorldDynamic);

	InteractionCapsule->SetCollisionResponseToAllChannels(ECR_Ignore);

	InteractionCapsule->SetCollisionResponseToChannel(
		ECC_GameTraceChannel1, ECR_Overlap);

	InteractionCapsule->SetGenerateOverlapEvents(true);

	//  seting your project's Interactable channel to Overlap
	// on this component in BP_FixedPointCharacter.

	
}

void AFixedPointCharacter::BindInteractionCapsuleEvents()
{
	InteractionCapsule->OnComponentBeginOverlap.AddUniqueDynamic(
		this, &AFixedPointCharacter::OnInteractionCapsuleBeginOverlap);

	InteractionCapsule->OnComponentEndOverlap.AddUniqueDynamic(
		this, &AFixedPointCharacter::OnInteractionCapsuleEndOverlap);
}

void AFixedPointCharacter::OnInteractionCapsuleBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	

	if (!IsLocallyControlled() ||
		!IsValid(OtherActor) ||
		OtherActor == this ||
		!IsValid(OtherComp) ||
		!OtherActor->GetClass()->ImplementsInterface(
			UFixedPointInteractableInterface::StaticClass()))
	{
		return;
	}

	

	AvailableInteractables.AddUnique(OtherActor);

	


	UE_LOG(LogFixedPointCharacter, Log, TEXT("Added Valid interaction candidate: %s"),*OtherActor->GetName());
}

void AFixedPointCharacter::OnInteractionCapsuleEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!IsLocallyControlled() ||
		!IsValid(OtherActor) ||
		OtherActor == this ||
		!OtherActor->GetClass()->ImplementsInterface(
			UFixedPointInteractableInterface::StaticClass()))
	{
		return;
	}
	

	AvailableInteractables.Remove(OtherActor);

	UE_LOG(LogFixedPointCharacter, Log, TEXT(" Removed Valid Interaction candidate: %s"),*OtherActor->GetName());


}

AActor* AFixedPointCharacter::FindMostAlignedInteractableActor() const
{
	if (!IsValid(PlayerCamera))
	{
		return nullptr;
	}

	UWorld* World = GetWorld();

	if (!IsValid(World))
	{
		return nullptr;
	}

	const FVector CameraLocation =
		PlayerCamera->GetComponentLocation();

	const FVector CameraForward =
		PlayerCamera->GetForwardVector();

	AActor* BestActor = nullptr;

	float BestAlignment = -1.0f;

	FCollisionQueryParams QueryParameters;

	QueryParameters.AddIgnoredActor(this);

	for (const TObjectPtr<AActor>& CandidatePtr : AvailableInteractables)
	{
		AActor* Candidate = CandidatePtr.Get();

		if (!IsValid(Candidate))
		{
			continue;
		}

		FVector TargetLocation;

		FVector BoundsExtent;

		Candidate->GetActorBounds(
			true,
			TargetLocation,
			BoundsExtent);

		const FVector DirectionToCandidate =
			(TargetLocation - CameraLocation).GetSafeNormal();

		const float AimAlignment =
			FVector::DotProduct(
				CameraForward,
				DirectionToCandidate);

		if (AimAlignment <= 0.0f)
		{
			continue;
		}

		FHitResult HitResult;

		const bool bHitSomething =
			World->LineTraceSingleByChannel(
				HitResult,
				CameraLocation,
				TargetLocation,
				ECC_Visibility,
				QueryParameters);

		if (bHitSomething &&
			HitResult.GetActor() != Candidate)
		{
			continue;
		}

		if (AimAlignment > BestAlignment)
		{
			BestAlignment = AimAlignment;

			BestActor = Candidate;
		}
	}

	return BestActor;
}

void AFixedPointCharacter::TryInteract()
{
	FocusedInteractable =
		FindMostAlignedInteractableActor();

	if (!IsValid(FocusedInteractable))
	{
		UE_LOG(LogFixedPointCharacter,Warning,TEXT("No interactable selected"));

		return;
	}


	ServerTryInteract(FocusedInteractable);

	// Rest of your debug code...

	FVector BoundsOrigin;
	FVector BoundsExtent;

	FocusedInteractable->GetActorBounds(
		true,
		BoundsOrigin,
		BoundsExtent);

	DrawDebugBox(
		GetWorld(),
		BoundsOrigin,
		BoundsExtent + FVector(5.0f, 5.0f, 5.0f),
		FColor::Green,
		false,
		2.0f,
		0,
		4.0f);

	

	DrawDebugString(
		GetWorld(),
		BoundsOrigin + FVector(0.0f, 0.0f, BoundsExtent.Z + 20.0f),
		FocusedInteractable->GetName(),
		nullptr,
		FColor::Green,
		2.0f,
		true);

	UE_LOG(LogFixedPointCharacter,Log,TEXT("Selected interactable: %s"),*FocusedInteractable->GetName());
}

bool AFixedPointCharacter::IsValidInteractionTarget(const AActor* RequestedTarget) const
{
	// is this being executed on the server 
	if (!HasAuthority())
	{
		UE_LOG(LogFixedPointCharacter, Warning, TEXT("Interaction Rejected: Validation was called without Authority."));

		return false;	
	}

	//is the interactable valid
	if (!IsValid(RequestedTarget))
	{
		UE_LOG(LogFixedPointCharacter, Warning, TEXT("Interaction rejected: requested target is invalid."));

		return false;
	}

	// are we interacting with ourself
	if (RequestedTarget == this)
	{
		UE_LOG(LogFixedPointCharacter,Warning,TEXT("Interaction rejected: character targeted itself."));

		return false;
	}

	//does the interactable implement the interactable interface

	if (!RequestedTarget->GetClass()->ImplementsInterface(UFixedPointInteractableInterface::StaticClass()))
	{
		UE_LOG(LogFixedPointCharacter,Warning,TEXT("Interaction rejected: %s does not implement the interaction interface."),*GetNameSafe(RequestedTarget));

		return false;
	}

	UWorld* World = GetWorld();

	// are we in a valid world to do a line trace
	if (!IsValid(World))
	{
		UE_LOG(LogFixedPointCharacter,Warning,TEXT("Interaction rejected: server world is invalid."));

		return false;
	}

	// is the player camera valid

	if (!IsValid(PlayerCamera))
	{
		UE_LOG(LogFixedPointCharacter, Warning, TEXT("Interaction rejected: PlayerCamera is invalid."));

		return false;

	}



	const FVector ViewLocation = PlayerCamera->GetComponentLocation();

	const FVector ViewDirection = GetBaseAimRotation().Vector();

	FVector TargetLocation;

	FVector TargetExtent;

	RequestedTarget->GetActorBounds(
		true,
		TargetLocation,
		TargetExtent);

	const FVector ViewToTarget = TargetLocation - ViewLocation;

	const float DistanceSquared = ViewToTarget.SizeSquared(); 
	// square the x y and z then add the squared sums together to give u the squared distance 

	const float MaxDistanceSquared = FMath::Square(MaxInteractionDistance);
	// u must also square this distance as you have done the other or else ur comparing unequivalent values


	if (DistanceSquared > MaxDistanceSquared) // return because its to far away to interact with
	{
		UE_LOG(LogFixedPointCharacter,Warning,TEXT("Interaction rejected: %s is %.1f units away; maximum is %.1f."),*GetNameSafe(RequestedTarget),FMath::Sqrt(DistanceSquared),MaxInteractionDistance);

		return false;

	}

	const  FVector DirectionToTarget = ViewToTarget.GetSafeNormal(); // takes the vector eliminates distance while keeping direction

	const float AimAlignment = FVector::DotProduct(ViewDirection, DirectionToTarget);

	if (AimAlignment < MinimumInteractionAlignment) // if were not rotated far enough towards the object were trying to interact with
	{
		UE_LOG(LogFixedPointCharacter,Warning,TEXT(	"Interaction rejected: %s has alignment %.2f; minimum is %.2f."),*GetNameSafe(RequestedTarget),AimAlignment,	MinimumInteractionAlignment);

		return false;
	}

	FHitResult HitResult;
	FCollisionQueryParams QueryParameters;

	QueryParameters.AddIgnoredActor(this);

	const bool bHitSomething =
		World->LineTraceSingleByChannel(
			HitResult,
			ViewLocation,
			TargetLocation,
			ECC_Visibility,
			QueryParameters);

	if (bHitSomething && HitResult.GetActor() != RequestedTarget)
	{
		UE_LOG(LogFixedPointCharacter,Warning,TEXT(	"Interaction rejected: %s blocks visibility to %s."),*GetNameSafe(HitResult.GetActor()),*GetNameSafe(RequestedTarget));

		return false;
	}

	return true;




}

void AFixedPointCharacter::ServerTryInteract_Implementation(
	AActor* RequestedTarget)
{
	if (!IsValidInteractionTarget(RequestedTarget))
	{
		return;
	}

	if (!IFixedPointInteractableInterface::Execute_CanInteract(
		RequestedTarget,
		this))
	{
		return;
	}

	IFixedPointInteractableInterface::Execute_Interact(
		RequestedTarget,
		this);

	
}

//INVENTORY 

void AFixedPointCharacter::InitializeInventoryComponent()
{
	InventoryComponent = CreateDefaultSubobject<UFixedPointInventoryComponent>(TEXT("Inventory Component"));
}

// CAMERA

void AFixedPointCharacter::InitializeCameraComponent()
{
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));

	PlayerCamera->SetupAttachment(GetCapsuleComponent());

	// Approximately eye height for the default Character capsule.
	// This can be tuned later from the character Blueprint.
	PlayerCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 74.0f));

	// Pitch and yaw come from the owning PlayerController's control rotation.
	PlayerCamera->bUsePawnControlRotation = true;
}
