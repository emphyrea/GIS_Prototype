// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Player/FightingTempCharacter.h"
#include "FightingTempGameMode.generated.h"

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	VE_Default UMETA(DisplayName = "Mocap"),
	VE_Mannequin UMETA(DisplayName = "Mannequin")
};

UCLASS(minimalapi)
class AFightingTempGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFightingTempGameMode();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Player References")
	AFightingTempCharacter* player1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Player References")
	AFightingTempCharacter* player2;
};



