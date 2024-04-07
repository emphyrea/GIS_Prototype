// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyBaseGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class UMyBaseGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Controller")
	bool isDeviceForMultiplePlayers;
};
