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
#include "FightingTemp/FightingTempGameMode.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AFightingTempCharacter

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

	PlayerHealth = 1.0f;
}

void AFightingTempCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
/*
	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		InputSubsystem->ClearAllMappings();
		InputSubsystem->AddMappingContext(DefaultMappingContext, 0);
	}
	*/
}

void AFightingTempCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
/*
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
		
	}
	*/
}

//////////////////////////////////////////////////////////////////////////
// Input

void AFightingTempCharacter::TakeDamage(float dmgAmt)
{
	PlayerHealth -= dmgAmt;
	if(PlayerHealth < 0.0f)
	{
		PlayerHealth = 0.0f;
	}
}

void AFightingTempCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if(auto gameMode = Cast<AFightingTempGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if(gameMode->player1 == this)
		{
			PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
			PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
			PlayerInputComponent->BindAxis("MoveRight", this, &AFightingTempCharacter::MoveRight);
			PlayerInputComponent->BindAction("Attack1", IE_Pressed, this, &AFightingTempCharacter::StartAttack1);
			/*if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
				// Jumping
				EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
				EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

				// Moving
				EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFightingTempCharacter::Move);

				//Attacking
				EnhancedInputComponent->BindAction(Attack1Action, ETriggerEvent::Started, this, &AFightingTempCharacter::StartAttack1);
				EnhancedInputComponent->BindAction(RangedAttackAction, ETriggerEvent::Triggered, this, &AFightingTempCharacter::StartRangedAttack1);
				EnhancedInputComponent->BindAction(GrabAction, ETriggerEvent::Triggered, this, &AFightingTempCharacter::StartGrab1);
				EnhancedInputComponent->BindAction(FinisherAction, ETriggerEvent::Started, this, &AFightingTempCharacter::StartFinisher1);

				// Looking
				//EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFightingTempCharacter::Look);
			}
			
			else
			{
				UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
			}*/
		}
		else
		{
			PlayerInputComponent->BindAction("JumpP2", IE_Pressed, this, &ACharacter::Jump);
			PlayerInputComponent->BindAction("JumpP2", IE_Released, this, &ACharacter::StopJumping);
			PlayerInputComponent->BindAxis("MoveRightP2", this, &AFightingTempCharacter::MoveRight);
			PlayerInputComponent->BindAction("Attack1P2", IE_Pressed, this, &AFightingTempCharacter::StartAttack1);
			/*
			if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
				// Jumping
				EnhancedInputComponent->BindAction(JumpActionP2, ETriggerEvent::Started, this, &ACharacter::Jump);
				EnhancedInputComponent->BindAction(JumpActionP2, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

				// Moving
				PlayerInputComponent->BindAxis("MoveRightP2", this, &AFightingTempCharacter::MoveRight);

				//Attacking
				EnhancedInputComponent->BindAction(Attack1ActionP2, ETriggerEvent::Started, this, &AFightingTempCharacter::StartAttack1);
				//EnhancedInputComponent->BindAction(RangedAttackAction, ETriggerEvent::Triggered, this, &AFightingTempCharacter::StartRangedAttack1);
				//EnhancedInputComponent->BindAction(GrabAction, ETriggerEvent::Triggered, this, &AFightingTempCharacter::StartGrab1);
				//EnhancedInputComponent->BindAction(FinisherAction, ETriggerEvent::Started, this, &AFightingTempCharacter::StartFinisher1);

				// Looking
				//EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFightingTempCharacter::Look);
			}
			else
			{
				UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
			}
			*/
		}
	}
}

void AFightingTempCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		//const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		//AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(FVector(1,0,0), MovementVector.X);
	}
}

void AFightingTempCharacter::MoveRight(float Value)
{
 AddMovementInput(FVector(-1.f, 0.f, 0.f), Value);
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

void AFightingTempCharacter::StartAttack1()
{
	UE_LOG(LogTemp, Warning, TEXT("First attack used!"));
	FirstAttackUsed = true;
}

void AFightingTempCharacter::StartRangedAttack1()
{
}

void AFightingTempCharacter::StartGrab1()
{
}

void AFightingTempCharacter::StartFinisher1()
{
	UE_LOG(LogTemp, Warning, TEXT("Finisher attack used!"));

}

void AFightingTempCharacter::P2KeyboardAttack1()
{
	StartAttack1();
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
