// Fill out your copyright notice in the Description page of Project Settings.

#include "FixedPointCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "../MovementComponents/FixedPointMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../../Interfaces/FixedPointInteractableInterface.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Controller.h"

// LIFE CYCLE

AFixedPointCharacter::AFixedPointCharacter(
	const FObjectInitializer& ObjectInitializer)
	: Super(
		ObjectInitializer.SetDefaultSubobjectClass<UFixedPointMovementComponent>(
			ACharacter::CharacterMovementComponentName))
{
	InitializeMovementComponent();

	InitializeCameraComponent();

	InitializeInteractionCapsuleComponent();

	
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

	


	UE_LOG(LogTemp, Log, TEXT("Added Valid interaction candidate: %s"),
		*OtherActor->GetName());
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

	UE_LOG(LogTemp, Log, TEXT(" Removed Valid Interaction candidate: %s"),
		*OtherActor->GetName());


}

AActor* AFixedPointCharacter::FindMostAlignedInteractableActor() const
{
	if (!IsValid(PlayerCamera))
	{
		return nullptr;
	}

	const FVector CameraLocation =
		PlayerCamera->GetComponentLocation();

	const FVector CameraForward =
		PlayerCamera->GetForwardVector();

	AActor* BestActor = nullptr;
	float BestAlignment = -1.0f;

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

		const float AimAlignment = FVector::DotProduct(
			CameraForward,
			DirectionToCandidate);

		if (AimAlignment <= 0.0f)
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
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("No interactable selected"));

		return;
	}

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

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Selected interactable: %s"),
		*FocusedInteractable->GetName());
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
