// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "FixedPointPlayerController.generated.h"

class AFixedPointCharacter;
class UInputAction;
class UInputMappingContext;

UCLASS()
class FIXEDPOINT_API AFixedPointPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// These functions give a future settings menu a clean way to change
	// the active hold/toggle preferences at runtime.
	UFUNCTION(BlueprintCallable, Category = "Input|Preferences")
	void SetCrouchToggleEnabled(bool bEnabled);

	UFUNCTION(BlueprintPure, Category = "Input|Preferences")
	bool IsCrouchToggleEnabled() const;

	UFUNCTION(BlueprintCallable, Category = "Input|Preferences")
	void SetSprintToggleEnabled(bool bEnabled);

	UFUNCTION(BlueprintPure, Category = "Input|Preferences")
	bool IsSprintToggleEnabled() const;

protected:
	//=====================================================
	// LIFE CYCLE
	//=====================================================

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	//=====================================================
	// INPUT ASSETS
	//=====================================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> PlayerMappingContext = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	int32 PlayerMappingPriority = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Actions")
	TObjectPtr<UInputAction> IA_Look = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Actions")
	TObjectPtr<UInputAction> IA_Move = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Actions")
	TObjectPtr<UInputAction> IA_Jump = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Actions")
	TObjectPtr<UInputAction> IA_Sprint = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Actions")
	TObjectPtr<UInputAction> IA_Crouch = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Actions")
	TObjectPtr<UInputAction> IA_Interact = nullptr;

	//=====================================================
	// INPUT PREFERENCES
	//=====================================================

	// New players use toggle crouch by default.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Preferences")
	bool bCrouchToggleEnabled = true;

	// New players use hold-to-sprint by default.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Preferences")
	bool bSprintToggleEnabled = false;

	//=====================================================
	// INPUT HANDLERS
	//=====================================================

	void HandleLook(const FInputActionValue& Value);
	void HandleMove(const FInputActionValue& Value);

	void HandleJumpStarted();
	void HandleJumpEnded();

	void HandleSprintStarted();
	void HandleSprintEnded();

	void HandleCrouchStarted();
	void HandleCrouchEnded();

	void HandleInteract();

private:
	AFixedPointCharacter* GetFixedPointCharacter() const;
};
