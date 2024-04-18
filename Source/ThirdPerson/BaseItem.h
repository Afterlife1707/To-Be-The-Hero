// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

UENUM()
enum EItemType
{
    Weapon,
	Mana,
	Throwable,
	Others
};

UCLASS()
class THIRDPERSON_API ABaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseItem();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ItemType")
	bool bIsBaseItem = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ItemType")
	TEnumAsByte<EItemType> ItemType = EItemType::Others;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ItemType")
	TArray<TSubclassOf<ABaseItem>> AllChildClasses;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Component")
    class UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USkeletalMeshComponent* SkeletalMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
