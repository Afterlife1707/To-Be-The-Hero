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
	Farmer,
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ThrowAction;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECharacterClass> CharacterType = ECharacterClass::Default;

	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypesArray;
	TArray<TObjectPtr<AActor>> actorsToIgnore;

public:
	AThirdPersonCharacter();

	UFUNCTION(BlueprintGetter)
	TEnumAsByte<ECharacterClass> GetCharacterType() const { return CharacterType; }

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = Combat)
	TEnumAsByte<EItemType> CurrentItem = EItemType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	TObjectPtr<AActor> Throwable;

	UPROPERTY(EditAnywhere, Category=Combat)
	float FarmerWeaponThrowMultiplier = 1000.f;
    UPROPERTY(EditAnywhere, Category = Combat)
	float FarmerThrowableMultiplier = 1500.f;
    UPROPERTY(EditAnywhere, Category = Combat)
	float KnightWeaponThrowMultiplier = 1500.f;
    UPROPERTY(EditAnywhere, Category = Combat)
	float KnightThrowableMultiplier = 2000.f;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, Replicated, meta = (AllowPrivateAccess = "true"))
	bool bIsAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, Replicated, meta = (AllowPrivateAccess = "true"))
	bool bIsCastingSpell;

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

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

