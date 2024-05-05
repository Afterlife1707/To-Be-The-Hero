// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerState.h"

#include "Net/UnrealNetwork.h"

AMyPlayerState::AMyPlayerState()
{
}

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMyPlayerState, bQualifiedForNextRound);
}

void AMyPlayerState::CopyProperties(APlayerState* NewPlayerState)
{
    Super::CopyProperties(NewPlayerState);

    if (AMyPlayerState* MyNewPlayerState = Cast<AMyPlayerState>(NewPlayerState))
    {
        bQualifiedForNextRound = MyNewPlayerState->bQualifiedForNextRound;
    }
}

void AMyPlayerState::OverrideWith(APlayerState* OldPlayerState)
{
    Super::OverrideWith(OldPlayerState);

    if (AMyPlayerState* MyOldPlayerState = Cast<AMyPlayerState>(OldPlayerState))
    {
        bQualifiedForNextRound = MyOldPlayerState->bQualifiedForNextRound;
    }
}