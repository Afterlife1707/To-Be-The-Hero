// Fill out your copyright notice in the Description page of Project Settings.


#include "MyLift.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

AMyLift::AMyLift()
{
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    bReplicates = true;
    bStaticMeshReplicateMovement = true;
    NetCullDistanceSquared = 400000000.0;
}

void AMyLift::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMyLift, Pos1);
    DOREPLIFETIME(AMyLift, Pos2);
}

void AMyLift::BeginPlay()
{
    Super::BeginPlay();

    UWorld* World = GetWorld();
    if (World && UKismetSystemLibrary::IsDedicatedServer(World) || UKismetSystemLibrary::IsServer(World))
    {
        GetWorldTimerManager().SetTimer(MovementTimerHandle, this, &AMyLift::SR_Move, MovementInterval, true);
    }
}

void AMyLift::SR_Move_Implementation()
{
    MoveToPosition(Pos1);
    Wait(WaitFor);
    MoveToPosition(Pos2);
    Wait(WaitFor);
    MoveToPosition(Pos1);
    Wait(WaitFor);
}

void AMyLift::MoveToPosition(const FVector& TargetPosition)
{
    UWorld* World = GetWorld();
    if (!World)
        return;

    FLatentActionInfo LatentAction;
    LatentAction.CallbackTarget = this;
    UKismetSystemLibrary::MoveComponentTo(Mesh, TargetPosition, FRotator::ZeroRotator, false, false, WaitFor, false, EMoveComponentAction::Move, LatentAction);
}

void AMyLift::Wait(float WaitDuration)
{
    UWorld* World = GetWorld();
    if (!World)
        return;

    FLatentActionInfo LatentAction;
    LatentAction.CallbackTarget = this;
    UKismetSystemLibrary::Delay(World, WaitDuration, LatentAction);
}
