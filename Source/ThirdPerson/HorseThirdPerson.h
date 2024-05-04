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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mount, meta = (AllowPrivateAccess = "true"))
    USceneComponent* UnmountPos;

public:
    AHorseThirdPerson();

    // Function to handle mounting the horse
    void MountHorse(AController* RiderController);

    // Function to handle dismounting the horse
    void DismountHorse();

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mount, Replicated, meta = (AllowPrivateAccess = "true"))
    bool bIsMounted;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mount, ReplicatedUsing = OnRep_RiderController, meta = (AllowPrivateAccess = "true"))
    AController* RiderController;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mount, Replicated, meta = (AllowPrivateAccess = "true"))
    APawn* OldPawn;

    UFUNCTION()
    void OnRep_RiderController();

};
