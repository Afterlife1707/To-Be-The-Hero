// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThirdPersonCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HorseThirdPerson.h"
#include "InputActionValue.h"
#include "KismetTraceUtils.h"
#include "Components/BoxComponent.h"
#include "Components/ProgressBar.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AThirdPersonCharacter

AThirdPersonCharacter::AThirdPersonCharacter()
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
	CameraBoom->SetupAttachment(GetMesh(), FName("Mesh"));

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	objectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	actorsToIgnore.Add(this);
}

void AThirdPersonCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AThirdPersonCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AThirdPersonCharacter, CurrentItem);
	DOREPLIFETIME(AThirdPersonCharacter, bIsAttacking);
	DOREPLIFETIME(AThirdPersonCharacter, bIsCastingSpell);
	DOREPLIFETIME(AThirdPersonCharacter, bIsThrowing);
	DOREPLIFETIME(AThirdPersonCharacter, CurrentCharacterType);
	DOREPLIFETIME(AThirdPersonCharacter, bIsRiding);
	DOREPLIFETIME(AThirdPersonCharacter, bIsInWater);
	DOREPLIFETIME(AThirdPersonCharacter, bHitByFairy);
	DOREPLIFETIME(AThirdPersonCharacter, bHasReachedFinishLine);
}
//////////////////////////////////////////////////////////////////////////
// Input

void AThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Move);

		// Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Ongoing, this, &AThirdPersonCharacter::Sprint);

		// Unsprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::None, this, &AThirdPersonCharacter::UnSprint);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Look);

		//left click
		// Attack
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::AttackServer);

		//right click
		// Throw //cast spell for wizard
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::ThrowServer);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}
#pragma region MOVEMENT

void AThirdPersonCharacter::Jump()
{
	if (bIsThrowing || bIsAttacking || bIsCastingSpell || bHasReachedFinishLine)
		return;
	bPressedJump = true;
	JumpKeyHoldTime = 0.0f;
}

void AThirdPersonCharacter::Move(const FInputActionValue& Value)
{
	if (bIsThrowing || bIsAttacking || bIsCastingSpell || bHasReachedFinishLine)
		return;
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr && !bIsAttacking)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement
		AddMovementInput(bHitByFairy ? -ForwardDirection : ForwardDirection, MovementVector.Y);
		AddMovementInput(bHitByFairy ? -RightDirection:RightDirection, MovementVector.X);
	}
}

void AThirdPersonCharacter::Look(const FInputActionValue& Value)
{
	if (bIsAttacking || bIsThrowing || bIsCastingSpell || bHasReachedFinishLine)
		return;
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(bHitByFairy ? -LookAxisVector.X : LookAxisVector.X);
		AddControllerPitchInput(bHitByFairy ? -LookAxisVector.Y: LookAxisVector.Y);
	}
}

void AThirdPersonCharacter::Sprint()
{
	if(bIsInWater)
	    GetCharacterMovement()->MaxWalkSpeed = WaterSprintSpeed;
	else
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	SprintServer();
}

void AThirdPersonCharacter::UnSprint()
{
	if (bIsInWater)
		GetCharacterMovement()->MaxWalkSpeed = WaterWalkSpeed;
	else
	    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	UnsprintServer();
}

void AThirdPersonCharacter::SprintServer_Implementation()
{
	if (bIsInWater)
		GetCharacterMovement()->MaxWalkSpeed = WaterSprintSpeed;
	else
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AThirdPersonCharacter::UnsprintServer_Implementation()
{
	if (bIsInWater)
		GetCharacterMovement()->MaxWalkSpeed = WaterWalkSpeed;
	else
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

#pragma endregion

#pragma region ATTACK
void AThirdPersonCharacter::AttackServer_Implementation()
{
	AttackMulti();
}

void AThirdPersonCharacter::AttackMulti_Implementation()
{
	if (bIsThrowing || bIsAttacking || GetCharacterMovement()->IsFalling() || bHasReachedFinishLine)
		return;

	if (CurrentItem == EItemType::Throwable)
		return;
	if (CurrentCharacterType == ECharacterClass::Rogue && CurrentItem == EItemType::Weapon)
		return;

	bIsAttacking = true;
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AThirdPersonCharacter::Melee, .5f, false);
}

void AThirdPersonCharacter::Melee()
{
	//UE_LOG(LogTemp, Warning, TEXT("left click"));
	FVector location = GetMesh()->GetComponentLocation();
	FVector rotation = UKismetMathLibrary::GetRightVector(GetMesh()->GetComponentRotation());

	FHitResult hit;
	bool bCollideObject = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), location, location + (rotation * 100), 20.f, objectTypesArray, false, actorsToIgnore, EDrawDebugTrace::None, hit, true);

	PlayAttackSoundEffect(); //blueprint function

	//UE_LOG(LogTemp, Warning, TEXT("hit is %s"), (bCollideObject ? TEXT("true") : TEXT("false")));
	if (bCollideObject)
	{
		if (auto otherActor = Cast<AThirdPersonCharacter>(hit.GetActor()))
		{
			PlayGotHitSoundEffect(hit.GetActor());
			otherActor->LaunchCharacter(rotation * 400, false, false);
		}
	}

	bIsAttacking = false;
}
#pragma endregion

#pragma region THROW

void AThirdPersonCharacter::ThrowServer_Implementation()
{
	if (bIsThrowing || bIsAttacking || bIsCastingSpell || GetCharacterMovement()->IsFalling() || bHasReachedFinishLine)
		return;
	//UE_LOG(LogTemp, Warning, TEXT("right click"));

	if (CurrentCharacterType == ECharacterClass::Wizard)
	{
		if (Mana == 0)
			return;
		if(!bIsCastingSpell)
		{
			bIsCastingSpell = true;
			ThrowMulticast();
			return;
		}
	}

	if (CurrentItem == EItemType::None) //if no items and not wizard, return
		return;

	bIsThrowing = true;
	ThrowMulticast();
}


bool AThirdPersonCharacter::ThrowServer_Validate()
{
	return !bIsAttacking;
}

void AThirdPersonCharacter::ThrowMulticast_Implementation()
{
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AThirdPersonCharacter::TriggerThrow, .8f, false);
}

void AThirdPersonCharacter::TriggerThrow()
{
	//UE_LOG(LogTemp, Warning, TEXT("bool %d"), bIsCastingSpell);
	if (CurrentCharacterType == ECharacterClass::Wizard)
	{
		UE_LOG(LogTemp, Warning, TEXT("spell cast"));
		CastSpell(); //blueprint function
		bIsCastingSpell = false;
		if (--Mana <= 0) Mana = 0;
		UpdateManaUI();
		return;
	}

	if (Throwable && CurrentItem!=None)
	{
		PlayAttackSoundEffect(); //blueprint function
		Throwable->SetActorEnableCollision(true);
        UBoxComponent* boxComp = Cast<UBoxComponent>(Throwable->GetRootComponent());
		if (boxComp)
		{
            const FVector PlayerLocation = GetActorForwardVector();
			boxComp->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale, "throwSocket");
			boxComp->SetSimulatePhysics(true);
			boxComp->SetCollisionProfileName("BlockAll");
			float ThrowMultiplier = 1000.f; //default val
			switch (CurrentItem)
			{
			    case EItemType::Throwable:
				    ThrowMultiplier = (CurrentCharacterType == ECharacterClass::Rogue)?RogueThrowableMultiplier:KnightThrowableMultiplier;
				    break;
			    case EItemType::Weapon:
				    ThrowMultiplier = (CurrentCharacterType == ECharacterClass::Rogue)?RogueWeaponThrowMultiplier:KnightWeaponThrowMultiplier;
				    break;
			}
			if (auto item = Cast<ABaseItem>(Throwable)) item->InstigatorForwardVector = GetActorForwardVector();
		    boxComp->SetPhysicsLinearVelocity(PlayerLocation.GetSafeNormal() * ThrowMultiplier);
		}
		AActor* ThrowableActor = Throwable;

	    FTimerHandle TimerHandle;
	    GetWorldTimerManager().SetTimer(TimerHandle, [ThrowableActor]() { ThrowableActor->Destroy(); }, 5.f, false);
	}

	bIsThrowing = false;
	CurrentItem = None; // empty after throw
	Throwable = nullptr;
}

#pragma endregion

#pragma region HORSE
void AThirdPersonCharacter::IncrementMana()
{
	if (++Mana > MaxMana)
		Mana = MaxMana;
	UpdateManaUI();
}

void AThirdPersonCharacter::UpdateManaUI()
{
	if(ManaBar)
	    ManaBar->SetPercent(Mana / MaxMana);
}

void AThirdPersonCharacter::MountHorse(AHorseThirdPerson* Horse)
{
	if (Horse)
	{
		// Call the server function to mount the horse
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerMountHorse(Horse);
		}
		else
		{
			// If the server, directly call the mounting function
			Horse->MountHorse(GetController());
		}
	}
}

void AThirdPersonCharacter::DismountHorse(AHorseThirdPerson* Horse)
{
	if (Horse)
	{
		// Call the server function to dismount the horse
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerDismountHorse(Horse);
		}
		else
		{
			// If the server, directly call the dismounting function
			Horse->DismountHorse();
		}
	}
}

void AThirdPersonCharacter::ServerMountHorse_Implementation(AHorseThirdPerson* Horse)
{
	MountHorse(Horse);
	bIsRiding = true;
}

void AThirdPersonCharacter::ServerDismountHorse_Implementation(AHorseThirdPerson* Horse)
{
	DismountHorse(Horse);
	bIsRiding = false;
}

void AThirdPersonCharacter::OnRep_IsRiding()
{
	UE_LOG(LogTemp, Warning, TEXT("on rep riding"));
	if (!bIsRiding)
	{
		SetActorEnableCollision(true);
		GetCharacterMovement()->GravityScale = 1.f;
	}
	else
	{
		GetCharacterMovement()->Velocity = FVector::Zero();
		SetActorEnableCollision(false);
		GetCharacterMovement()->GravityScale = 0;
	}
}
#pragma endregion

void AThirdPersonCharacter::OnRep_IsInWater()
{
}