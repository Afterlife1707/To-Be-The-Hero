// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ThirdPersonCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;


UENUM(BlueprintType)
enum ECharacterClass
{
	Knight,
	Wizard,
	Rogue,
	Default
};

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AThirdPersonCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ThrowAction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"), Replicated)
	TEnumAsByte<ECharacterClass> CurrentCharacterType = ECharacterClass::Default;

	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypesArray;
	TArray<TObjectPtr<AActor>> actorsToIgnore;

	float Mana = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	float MaxMana = 3;

public:
	AThirdPersonCharacter();

	UFUNCTION(BlueprintGetter, Category = Combat)
	TEnumAsByte<ECharacterClass> GetCurrentCharacterType() const { return CurrentCharacterType; };

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = Combat)
	TEnumAsByte<EItemType> CurrentItem = EItemType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	TObjectPtr<AActor> Throwable;

	UPROPERTY(EditDefaultsOnly, Category=Combat)
	float RogueWeaponThrowMultiplier = 1000.f;
    UPROPERTY(EditDefaultsOnly, Category = Combat)
	float RogueThrowableMultiplier = 1500.f;
    UPROPERTY(EditDefaultsOnly, Category = Combat)
	float KnightWeaponThrowMultiplier = 1500.f;
    UPROPERTY(EditDefaultsOnly, Category = Combat)
	float KnightThrowableMultiplier = 2000.f;

	void IncrementMana();
	void UpdateManaUI();
	float GetMana() { return Mana; }
	float GetMaxMana() { return MaxMana; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	class UProgressBar* ManaBar;

	//HORSE
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Horse, ReplicatedUsing=OnRep_IsRiding, meta = (AllowPrivateAccess = "true"))
	bool bIsRiding;
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = Horse)
	void ServerMountHorse(class AHorseThirdPerson* Horse);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = Horse)
	void ServerDismountHorse(class AHorseThirdPerson* Horse);

	// Function called on input to mount the horse
	UFUNCTION(BlueprintCallable, Category = Horse)
	void MountHorse(class AHorseThirdPerson* Horse);

	// Function called on input to dismount the horse
	UFUNCTION(BlueprintCallable, Category = Horse)
	void DismountHorse(class AHorseThirdPerson* Horse);

	UFUNCTION(Category = Horse)
	void OnRep_IsRiding();

protected:

	virtual void Jump() override;
	/** Called for movement input */
	virtual void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for looking input */
	void Sprint();
	/** Called for looking input */
	void UnSprint();

	UFUNCTION(Server, Unreliable)
	void SprintServer();
    UFUNCTION(Server, Unreliable)
	void UnsprintServer();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float WalkSpeed = 150.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float SprintSpeed = 350.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float WaterWalkSpeed = 80.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float WaterSprintSpeed = 250.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, Replicated, meta = (AllowPrivateAccess = "true"))
	bool bIsAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, Replicated, meta = (AllowPrivateAccess = "true"))
	bool bIsCastingSpell;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, Replicated, meta = (AllowPrivateAccess = "true"))
	bool bIsThrowing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, ReplicatedUsing= OnRep_IsInWater, meta = (AllowPrivateAccess = "true"))
	bool bIsInWater;

	UFUNCTION(Category = Combat)
	void OnRep_IsInWater();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, Replicated, meta = (AllowPrivateAccess = "true"))
	bool bHitByFairy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, Replicated, meta = (AllowPrivateAccess = "true"))
	bool bHasReachedFinishLine;

	UFUNCTION(Server, Reliable)
	void AttackServer();
    UFUNCTION(NetMulticast, Reliable)
	void AttackMulti();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ThrowServer();
	UFUNCTION(NetMulticast, Reliable)
	void ThrowMulticast();
	void TriggerThrow();
	void Melee();
	UFUNCTION(BlueprintImplementableEvent, Category=Combat, meta = (AllowPrivateAccess = "true"))
	void CastSpell();

	UFUNCTION(BlueprintImplementableEvent, Category = Combat, meta = (AllowPrivateAccess = "true"))
	void PlayAttackSoundEffect();
	UFUNCTION(BlueprintImplementableEvent, Category = Combat, meta = (AllowPrivateAccess = "true"))
	void PlayGotHitSoundEffect(AActor* HitByActor);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

