// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItem.h"

#include "ThirdPersonCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABaseItem::ABaseItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));

	RootComponent = BoxCollision;
	StaticMesh->SetupAttachment(RootComponent);
	SkeletalMesh->SetupAttachment(RootComponent);
	bReplicates = true;
}



// Called when the game starts or when spawned
void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::ItemOverlapped);
	if (!HasAuthority())
		return;
	if(bIsBaseItem)
	{
		if (UKismetMathLibrary::RandomBool())
		{
			int32 rand = UKismetMathLibrary::RandomIntegerInRange(0, AllChildClasses.Num() - 1);
			const TSubclassOf<ABaseItem> RandomClassItemToSpawn = AllChildClasses[rand];
			UE_LOG(LogTemp, Warning, TEXT("Random val %d"), rand);
            if(ABaseItem* ItemRef = GetWorld()->SpawnActor<ABaseItem>(RandomClassItemToSpawn, GetActorTransform()))
				ItemRef->bIsBaseItem = false;
			Destroy();
		}
		else
			Destroy();
	}
}

// Called every frame
void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseItem::ItemOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(auto* Character = Cast<AThirdPersonCharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("Component hit"));
		if (bIsEquippable)
		{
			SetActorEnableCollision(false);
			if (StaticMesh->GetStaticMesh())
				StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "head");
		}
		else
		{
			//UPDATE MANA
			Destroy();
		}
	}
}
