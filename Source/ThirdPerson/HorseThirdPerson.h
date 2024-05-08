// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThirdPersonCharacter.h"
#include "HorseThirdPerson.generated.h"

/**
 *
 */
UCLASS()
class THIRDPERSON_API AHorseThirdPerson : public AThirdPersonCharacter
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Horse, meta = (AllowPrivateAccess = "true"))
    USceneComponent* UnmountPos;

public:
    AHorseThirdPerson();

    void MountHorse(AController* RiderController);
    void DismountHorse();

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
    virtual void Jump() override;
    void DelayedJump();
    virtual void Move(const FInputActionValue& Value) override;

private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Horse, Replicated, meta = (AllowPrivateAccess = "true"))
    bool bIsMounted;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Horse, ReplicatedUsing = OnRep_RiderController, meta = (AllowPrivateAccess = "true"))
    AController* RiderController;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Horse, Replicated, meta = (AllowPrivateAccess = "true"))
    APawn* OldPawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Horse, Replicated, meta = (AllowPrivateAccess = "true"))
    float ForwardThrustMultiplier = 1200.f;

    UFUNCTION()
    void OnRep_RiderController();

};
