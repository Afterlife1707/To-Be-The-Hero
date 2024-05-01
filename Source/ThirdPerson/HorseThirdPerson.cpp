// Fill out your copyright notice in the Description page of Project Settings.


#include "HorseThirdPerson.h"

#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"


AHorseThirdPerson::AHorseThirdPerson()
{
    SitPos = CreateDefaultSubobject<USceneComponent>(TEXT("SitPos"));
    UnmountPos = CreateDefaultSubobject<USceneComponent>(TEXT("UnmountPos"));

    SitPos->SetupAttachment(RootComponent);
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
        UE_LOG(LogTemp, Warning, TEXT("mount"));
        bIsMounted = true;
        OldPawn = NewRiderController->GetPawn();
        RiderController = NewRiderController;
        if(auto Character = Cast<AThirdPersonCharacter>(NewRiderController->GetCharacter()))
        {
            if(auto Capsule = Cast<UCapsuleComponent>(Character->GetRootComponent()))
            {
                //Capsule->SetEnableGravity(false);
                UE_LOG(LogTemp, Warning, TEXT("capsule"));
               // Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            }
            Character->SetActorEnableCollision(false);
            //Character->SetActorLocation(SitPos->GetComponentLocation());
            Character->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "sitSocket");
        }
        RiderController->Possess(this);
    }
}

void AHorseThirdPerson::DismountHorse()
{
    if (RiderController && bIsMounted)
    {
        UE_LOG(LogTemp, Warning, TEXT("dismount"));
        bIsMounted = false;
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
    // Handle replication of rider controller changes
    if (RiderController)
    {
        RiderController->Possess(this);
    }
}
