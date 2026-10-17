// Fill out your copyright notice in the Description page of Project Settings.


#include "FixedPointAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

// LIFECYLE
AFixedPointAIController::AFixedPointAIController(const FObjectInitializer& ObjectInitializer)
{
	//Create The perception component in memory and assign to are variable upon construction
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));

	SetPerceptionComponent(*PerceptionComponent);

	//create the sight configuration in memory
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Configuration"));

	//Set the default values for sight here 

	SightConfig->SightRadius = 3000.f;
	
	SightConfig->LoseSightRadius = 3500.f;

	SightConfig->PeripheralVisionAngleDegrees = 90.f;

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;

	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;

	// OTHER SETTABLE SIGHT CONFIG VALUES EXIST IF NEEDED 

	// Set up the sight config we have made
	GetAIPerceptionComponent()->ConfigureSense(*SightConfig);

	GetAIPerceptionComponent()->SetDominantSense(SightConfig->GetSenseImplementation());



}



void AFixedPointAIController::BeginPlay()
{
	Super::BeginPlay();
	 
	//bind the function we call we call when perception gets updated.
	GetAIPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AFixedPointAIController::OnTargetPercieved);

		


}

void AFixedPointAIController::Tick(float DeltaTime)
{

}

//PERCEPTION

// What Happens when the AI percieves an actor through any sense
void AFixedPointAIController::OnTargetPercieved(AActor* Actor, FAIStimulus Stimulus)
{

}