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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mount,meta = (AllowPrivateAccess = "true"))
    USceneComponent* SitPos;
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
    // Boolean to track if the horse is currently mounted
    UPROPERTY(EditAnywhere, Category=Mount, Replicated)
    bool bIsMounted;

    // Reference to the rider controller
    UPROPERTY(EditAnywhere, Category = Mount, ReplicatedUsing = OnRep_RiderController)
    AController* RiderController;

    APawn* OldPawn;

    // Replicated function to handle changes to the rider controller
    UFUNCTION()
    void OnRep_RiderController();

};
