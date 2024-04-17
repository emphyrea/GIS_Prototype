// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

//#include "GameFramework/Character.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "FightingTempCharacter.generated.h"

UENUM(BlueprintType)
enum class EDirectionalInput : uint8
{
	VE_Default UMETA(DisplayName = "NOT_MOVING"),
	VE_MovingRight UMETA(DisplayName = "MOVING_RIGHT"),
	VE_MovingLeft UMETA(DisplayName = "MOVING_LEFT")
};


class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AFightingTempCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//USpringArmComponent* CameraBoom;

	///** Follow camera */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//UCameraComponent* FollowCamera;


	//------NOTE THIS CHARACTER AND BLUEPRINT IS LOCAL MULTIPLAYER ONLY! ENTIRELY SERVER SIDE-----//
	
public:
	AFightingTempCharacter();

	virtual void PawnClientRestart() override;


protected:

	/*****************************************************/
	/*                    Actions                        */
	/*****************************************************/
	
	/** Called for movement input */
	void MoveRight(float Value);
	void MoveRightController(float Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	void StartAttackLight();
	void StartAttackHeavy();
	void StartAttackMedium();
	void StartAttackSpecial();
	void StartFinisher1();
	void StartBlock();

	UFUNCTION(BlueprintCallable)
	void P2KeyboardAttack1();

	UFUNCTION(BlueprintCallable)
	void P2KeyboardJump();
	
	UFUNCTION(BlueprintCallable)
	void P2KeyboardStopJumping();
	
	UFUNCTION(BlueprintCallable)
	void P2KeyboardMoveRight(float value);

	// ------------------------------- //

	void TakeDamage(float dmgAmt);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float PlayerHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Controller")
	bool isDeviceForMultiplePlayers;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Attacks")
	bool FirstAttackUsed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player References")
	AFightingTempCharacter* otherPlayer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Model")
	bool isFlipped;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Model")
	FTransform transform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Model")
	FVector scale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	EDirectionalInput directionalInput;

protected:

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

public:
	///** Returns CameraBoom subobject **/
	//FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	///** Returns FollowCamera subobject **/
	//FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

