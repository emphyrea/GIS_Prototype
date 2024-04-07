// Copyright Epic Games, Inc. All Rights Reserved.

#include "FightingTempCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTypes.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "GameplayTagsManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"

#include "InputActionValue.h"
#include "MyBaseGameInstance.h"
#include "FightingTemp/FightingTempGameMode.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AFightingTempCharacter

//------NOTE THIS CHARACTER AND BLUEPRINT IS LOCAL MULTIPLAYER ONLY! ENTIRELY SERVER SIDE-----//

AFightingTempCharacter::AFightingTempCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	FirstAttackUsed = false;
	directionalInput = EDirectionalInput::VE_Default;
	transform = FTransform(FQuat(), FVector(), FVector());
	scale = FVector(0.0f, 0.0f, 0.0f);
	PlayerHealth = 1.0f;
	isFlipped = false;
}

void AFightingTempCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
}

void AFightingTempCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
}

void AFightingTempCharacter::Tick(float DeltaSeconds) //the worst code ever possible is below, fix later PLEASE WTF
{
	Super::Tick(DeltaSeconds);
//HELP
	if(otherPlayer) //if exists....
	{
		if(auto characterMovement = GetCharacterMovement()) //then get our char movement
		{
			//WHY
			if(auto enemyMovement = otherPlayer->GetCharacterMovement()) //get other player's movement
			{
				//IT JUST KEEPS GOING
				if(enemyMovement->GetActorLocation().X <= characterMovement->GetActorLocation().X) //left and right <--> = X in unreal. on map check if enemy to left of u
				{
					if(isFlipped) //unflip
					{
						//sobbing 
						if(auto mesh = GetCapsuleComponent()->GetChildComponent(1)) //get tha meshh yass get it
						{
							transform = mesh->GetRelativeTransform();
							scale = transform.GetScale3D();
							scale.Y = -1;
							transform.SetScale3D(scale);
							mesh->SetRelativeTransform(transform);
						}
						isFlipped = false;
					}
				}
				else
				{
					if(!isFlipped) //flip
					{
						//sobbing 
						if(auto mesh = GetCapsuleComponent()->GetChildComponent(1)) //get tha meshh yass get it
						{
							transform = mesh->GetRelativeTransform();
							scale = transform.GetScale3D();
							scale.Y = 1;
							transform.SetScale3D(scale);
							mesh->SetRelativeTransform(transform);
						}
						isFlipped = true;
					}
					
				}
				
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Actions

void AFightingTempCharacter::TakeDamage(float dmgAmt)
{
	PlayerHealth -= dmgAmt;
	if(PlayerHealth < 0.0f)
	{
		PlayerHealth = 0.0f;
	}
	//TEMP!!!!
}

void AFightingTempCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if(auto gameMode = Cast<AFightingTempGameMode>(GetWorld()->GetAuthGameMode()))
	{
		// We are using the old input system due to issues with axis mapping equivalent not found in enhanced input system. --> needed for blueprint 
		if(gameMode->player1 == this)
		{
			PlayerInputComponent->BindAxis("MoveRight", this, &AFightingTempCharacter::MoveRight);
		}
		else
		{
			PlayerInputComponent->BindAxis("MoveRightP2", this, &AFightingTempCharacter::MoveRight);
		}
		PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
		PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
		PlayerInputComponent->BindAction("AttackLight", IE_Pressed, this, &AFightingTempCharacter::StartAttackLight);
		PlayerInputComponent->BindAxis("MoveRightController", this, &AFightingTempCharacter::MoveRightController);

		PlayerInputComponent->BindAction("JumpP2", IE_Pressed, this, &ACharacter::Jump);
		PlayerInputComponent->BindAction("JumpP2", IE_Released, this, &ACharacter::StopJumping);
		PlayerInputComponent->BindAction("AttackLightP2", IE_Pressed, this, &AFightingTempCharacter::StartAttackLight);
	}
}

void AFightingTempCharacter::MoveRight(float Value) //altered in blueprint btw for left most and right most player specific movement
{
	if(auto baseGameInstance = Cast<UMyBaseGameInstance>(GetGameInstance())) 
	{
		if(baseGameInstance->isDeviceForMultiplePlayers)
		{
			//if(canMove and not block) ,do the things i have not yet added lol
			//if not jumpingstate or launched --not added yet
			if(Value > 0.01f)
			{
				directionalInput = EDirectionalInput::VE_MovingRight;
			}
			else if (Value < -0.01f)
			{
				directionalInput = EDirectionalInput::VE_MovingLeft;
			}
			else
			{
				directionalInput = EDirectionalInput::VE_Default;
			}

			float currentDistanceApart = abs(otherPlayer->GetActorLocation().X - GetActorLocation().X);
			if(currentDistanceApart >= 5.0f)
			{
				if(((currentDistanceApart + Value < currentDistanceApart) && !isFlipped) || ((currentDistanceApart - Value < currentDistanceApart) && isFlipped))
				{
					AddMovementInput(FVector(1.f, 0.f, 0.f), Value);
				}
				else
				{
					AddMovementInput(FVector(1.f, 0.f, 0.f), Value);
				}
			}

		}
	}
}

void AFightingTempCharacter::MoveRightController(float Value)
{
	if(auto baseGameInstance = Cast<UMyBaseGameInstance>(GetGameInstance())) 
		{
			if(!baseGameInstance->isDeviceForMultiplePlayers)
			{
				//if(canMove and not block) ,do the things i have not yet added lol
				//if not jumpingstate or launched --not added yet
				if(Value > 0.20f)
				{
					directionalInput = EDirectionalInput::VE_MovingRight;
				}
				else if (Value < -0.20f)
				{
					directionalInput = EDirectionalInput::VE_MovingLeft;
				}
				else
				{
					directionalInput = EDirectionalInput::VE_Default;
				}
				float currentDistanceApart = abs(otherPlayer->GetActorLocation().X - GetActorLocation().X);
				if(currentDistanceApart >= 5.0f)
				{
					if(((currentDistanceApart + Value < currentDistanceApart) && !isFlipped) || ((currentDistanceApart - Value < currentDistanceApart) && isFlipped))
					{
						AddMovementInput(FVector(1.f, 0.f, 0.f), Value);
					}
					else
					{
						AddMovementInput(FVector(1.f, 0.f, 0.f), Value);
					}
				}

			}
		}
}

void AFightingTempCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFightingTempCharacter::StartAttackLight()
{
	UE_LOG(LogTemp, Warning, TEXT("First attack used!"));
	FirstAttackUsed = true;
	//TEMP
}

void AFightingTempCharacter::StartAttackMedium()
{
	//TEMP
}

void AFightingTempCharacter::StartAttackHeavy()
{
	//TEMP
}

void AFightingTempCharacter::StartFinisher1()
{
	UE_LOG(LogTemp, Warning, TEXT("Finisher attack used!"));
	//TEMP

}


void AFightingTempCharacter::StartAttackSpecial()
{
	UE_LOG(LogTemp, Warning, TEXT("Finisher attack used!"));
	//TEMP

}


void AFightingTempCharacter::StartBlock()
{
	//TEMP
}

//---------------KEYBOARD SPECIFIC PLAYER 2 ACTIONS BELOW----------- //

void AFightingTempCharacter::P2KeyboardAttack1()
{
	StartAttackLight();
}

void AFightingTempCharacter::P2KeyboardJump()
{
	Jump();
}

void AFightingTempCharacter::P2KeyboardStopJumping()
{
	StopJumping();
}

void AFightingTempCharacter::P2KeyboardMoveRight(float value)
{
	MoveRight(value);
}
