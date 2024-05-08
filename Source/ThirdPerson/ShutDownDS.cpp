// Fill out your copyright notice in the Description page of Project Settings.


#include "ShutDownDS.h"

void UShutDownDS::ShutDown()
{
	GIsRequestingExit = true;
}
