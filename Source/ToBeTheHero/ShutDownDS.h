// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ShutDownDS.generated.h"

/**
 * 
 */
UCLASS()
class TOBETHEHERO_API UShutDownDS : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "Custom", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", Keywords = "Shut Down"))
	static void ShutDown();

};
