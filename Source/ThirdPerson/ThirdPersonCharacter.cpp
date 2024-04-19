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
#include "InputActionValue.h"
#include "KismetTraceUtils.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/KismetMathLibrary.h"

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

//////////////////////////////////////////////////////////////////////////
// Input

void AThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Look);

		// Attack
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Attack);

		// Throw
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Throw);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AThirdPersonCharacter::Move(const FInputActionValue& Value)
{
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
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AThirdPersonCharacter::Look(const FInputActionValue& Value)
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

#pragma region ATTACK
void AThirdPersonCharacter::Attack()
{
	if (GetCharacterMovement()->IsFalling())
		return;
	if (CharacterType == ECharacterClass::Farmer)
	{
	    if (CurrentItem == EItemType::Weapon || CurrentItem == EItemType::Throwable)
		    return;
	}
	if (CharacterType == ECharacterClass::Knight && CurrentItem == EItemType::Throwable)
		return;
	//TODO Cannot attack notify user with UI in above cases
	AttackServer();
}

void AThirdPersonCharacter::AttackServer_Implementation()
{
	AttackMulticast();
}

void AThirdPersonCharacter::AttackMulticast_Implementation()
{
	if (bIsAttacking)
		return;

	bIsAttacking = true;

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AThirdPersonCharacter::TriggerAttack, .8f, false);
}

void AThirdPersonCharacter::TriggerAttack()
{
	//knight can attack and throw
	//peasant can only throw(both sword and throwable)
	//wizard can only pickup mana
	switch(CurrentItem)
	{
	    case EItemType::None:
	    {
			Melee();
			break;
	    }
	    case EItemType::Weapon://and class check knight here, play sword sound on hit
		{
			Melee();
			break;
		}
	    case EItemType::Throwable:
		{
			Throw();
			break;
		}
	    case EItemType::Mana:
		{
			CastSpell();
			break;
		}
	    default:
		{
			Melee();
			break;
		}
	}

	bIsAttacking = false;
}


void AThirdPersonCharacter::Melee()
{
	UE_LOG(LogTemp, Warning, TEXT("melee"));
	FVector location = GetMesh()->GetComponentLocation();
	FVector rotation = UKismetMathLibrary::GetRightVector(GetMesh()->GetComponentRotation());

	FHitResult hit;
	bool bCollideObject = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), location, location + (rotation * 100), 20.f, objectTypesArray, false, actorsToIgnore, EDrawDebugTrace::None, hit, true);

	UE_LOG(LogTemp, Warning, TEXT("hit is %s"), (bCollideObject ? TEXT("true") : TEXT("false")));
	if (bCollideObject)
	{
		if (auto otherActor = Cast<AThirdPersonCharacter>(hit.GetActor()))
		{
			UE_LOG(LogTemp, Warning, TEXT("other actor %s"), *otherActor->GetPlayerState()->GetPlayerName());
			otherActor->LaunchCharacter(rotation * 400, false, false);
		}
	}
}
#pragma endregion

void AThirdPersonCharacter::CastSpell()
{
	//TODO
	UE_LOG(LogTemp, Warning, TEXT("spell cast"));
}

#pragma region THROW
void AThirdPersonCharacter::Throw()
{
	if (GetCharacterMovement()->IsFalling() || CharacterType == ECharacterClass::Wizard || CurrentItem == EItemType::None)
		return;

	ThrowServer();
}

void AThirdPersonCharacter::ThrowServer_Implementation()
{
	ThrowMulticast();
}

void AThirdPersonCharacter::ThrowMulticast_Implementation()
{
	if (bIsAttacking)
		return;

	bIsAttacking = true;

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AThirdPersonCharacter::TriggerThrow, .8f, false);
}


void AThirdPersonCharacter::TriggerThrow()
{
	UE_LOG(LogTemp, Warning, TEXT("throw object"));
	if (Throwable)
	{
		Throwable->SetActorEnableCollision(true);
		//Throwable->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
        UBoxComponent* boxComp = Cast<UBoxComponent>(Throwable->GetRootComponent());
		if (boxComp)
		{
			const FVector PlayerLocation = GetActorForwardVector();
			boxComp->SetSimulatePhysics(true);
			boxComp->SetCollisionProfileName("BlockAll");
			boxComp->SetPhysicsLinearVelocity(PlayerLocation.GetSafeNormal() * 1000.f);
		}
		AActor* ThrowableActor = Throwable;
		//destroying both sword and throwable for now since it will cause issues with hitting player with sword
		//if(CurrentItem==EItemType::Throwable) //destroy if throwable thrown
		//{
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, [ThrowableActor]() { ThrowableActor->Destroy(); }, 5.f, false);
		//}
		//else //else renable overlap collision detection
		//{
		//	FTimerHandle TimerHandle;
		//	GetWorldTimerManager().SetTimer(TimerHandle, [boxComp]() {boxComp->SetCollisionProfileName("OverlapOnlyPawn"); }, 5.f, false);
		//}

	}

	bIsAttacking = false;
	CurrentItem = EItemType::None; // empty after throw
	Throwable = nullptr;
}

#pragma endregion
