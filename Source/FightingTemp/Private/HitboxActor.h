// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HitboxActor.generated.h"

UENUM(BlueprintType)
enum class EHitboxEnum : uint8
{
	//proximity hitbox here if needed
	HB_PROXIMITY  UMETA(DisplayName = "Proximity"),

	//projectile hitbox here when needed
	HB_STRIKE   UMETA(DisplayName = "Strike"),
	HB_HURTBOX  UMETA(DisplayName = "Hurtbox")
};

UCLASS()
class AHitboxActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHitboxActor();

	//notify HitboxActorBP class instance that hitbox is ready to be drawn
	UFUNCTION(BlueprintCallable)
	void TriggerVisualHitbox();

	//draw hitbox to screen (for debug)
	UFUNCTION(BlueprintImplementableEvent)
	void VisualizeHitbox();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Hitbox")
	float hitboxDamage;
	
	//hitbox enum instance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Hitbox")
	EHitboxEnum hitboxType;

	//location to spawn hitbox
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Hitbox")
	FVector hitboxLocation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
