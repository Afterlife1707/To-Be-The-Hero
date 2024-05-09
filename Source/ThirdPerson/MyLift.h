// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "MyLift.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API AMyLift : public AStaticMeshActor
{
	GENERATED_BODY()


public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;

protected:
	AMyLift();
    virtual void BeginPlay() override;

    void MoveToPosition(const FVector& Pos1);
    void Wait(float WaitDuration);
    UFUNCTION(Server, Reliable, Category = "Lift")
	void SR_Move();

private:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lift", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lift", Replicated, meta = (AllowPrivateAccess = "true"))
	FVector Pos1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lift", Replicated, meta = (AllowPrivateAccess = "true"))
	FVector Pos2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lift", meta = (AllowPrivateAccess = "true"))
	float WaitFor = 2.f;

    FTimerHandle MovementTimerHandle;
    float MovementInterval=5.f;
};
