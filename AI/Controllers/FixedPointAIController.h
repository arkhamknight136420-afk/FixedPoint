// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "FixedPointAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class FAIStimulus;
/**
 * 
 */
UCLASS()
class FIXEDPOINT_API AFixedPointAIController : public AAIController
{
	GENERATED_BODY()
	
public:

	//LIFECYCLE

	//PERCEPTION

protected:

	//LIFECYCLE

	//PERCEPTION

	void OnTargetPercieved(AActor* Actor, FAIStimulus Stimulus);

	UPROPERTY(BlueprintReadOnly, Category = "Perception")
	TObjectPtr<UAIPerceptionComponent> PerceptionComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig = nullptr;




private:

	//LIFECYCLE

	AFixedPointAIController(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	//PERCEPTION

};
