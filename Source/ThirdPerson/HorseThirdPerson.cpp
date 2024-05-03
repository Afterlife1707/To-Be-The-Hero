// Fill out your copyright notice in the Description page of Project Settings.


#include "HorseThirdPerson.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"


AHorseThirdPerson::AHorseThirdPerson()
{
    UnmountPos = CreateDefaultSubobject<USceneComponent>(TEXT("UnmountPos"));
    UnmountPos->SetupAttachment(RootComponent);

    bReplicates = true;
}

void AHorseThirdPerson::BeginPlay()
{
    Super::BeginPlay();
}

void AHorseThirdPerson::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AHorseThirdPerson, bIsMounted);
    DOREPLIFETIME(AHorseThirdPerson, RiderController);
}

void AHorseThirdPerson::MountHorse(AController* NewRiderController)
{
    if (NewRiderController && !bIsMounted && HasAuthority())
    {
        //UE_LOG(LogTemp, Warning, TEXT("mount"));
        bIsMounted = true;
        OldPawn = NewRiderController->GetPawn();
        RiderController = NewRiderController;
        if (auto Character = Cast<AThirdPersonCharacter>(NewRiderController->GetCharacter()))
        {
            Character->SetActorEnableCollision(false);
            Character->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "sitSocket");
        }
        RiderController->Possess(this);
    }
}

void AHorseThirdPerson::DismountHorse()
{
    if (RiderController && bIsMounted)
    {
        //UE_LOG(LogTemp, Warning, TEXT("dismount"));
        bIsMounted = false;
        GetCharacterMovement()->Velocity = FVector::Zero();
        if (auto Character = Cast<AThirdPersonCharacter>(OldPawn))
        {
            Character->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
            Character->SetActorEnableCollision(true);
            Character->SetActorLocation(UnmountPos->GetComponentLocation());
            Character->bIsRiding = false;
        }
        RiderController->UnPossess();
        RiderController->Possess(OldPawn);
        bIsRiding = false;
    }
}

void AHorseThirdPerson::OnRep_RiderController()
{
    if (RiderController)
    {
        RiderController->Possess(this);
    }
}
