// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TOBETHEHERO_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()

    AMyPlayerState();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void CopyProperties(APlayerState* NewPlayerState) override;
    virtual void OverrideWith(APlayerState* OldPlayerState) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, meta = (AllowPrivateAccess = "true"))
    bool bQualifiedForNextRound=true;
};
