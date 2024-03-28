// Fill out your copyright notice in the Description page of Project Settings.

#include "FightingTemp/Private/Player/CPlayerController.h"
#include "FightingTemp/Private/Player/FightingTempCharacter.h"


void ACPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	PostPossessionSetup(NewPawn);
	if (PlayerCharacter)
	{
		//PlayerCharacter->InitAbilities();
	}
}

void ACPlayerController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);
	if (!HasAuthority())
	{
		PostPossessionSetup(NewPawn);
	}
}

void ACPlayerController::PostPossessionSetup(APawn* NewPawn)
{
	PlayerCharacter = Cast<AFightingTempCharacter>(NewPawn);
	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("Needs CPlayerCharacter as Pawn"));
		return;
	}
	
}

