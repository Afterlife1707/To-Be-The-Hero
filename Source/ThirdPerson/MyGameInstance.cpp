// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "PlayfabGSDK.h"
#include "GSDKUtils.h"

DEFINE_LOG_CATEGORY(LogPlayFabGSDKGameInstance);

void UMyGameInstance::Init()
{
    if (IsDedicatedServerInstance() == true)
    {
        FOnGSDKShutdown_Dyn OnGsdkShutdown;
        OnGsdkShutdown.BindDynamic(this, &UMyGameInstance::OnGSDKShutdown);
        FOnGSDKHealthCheck_Dyn OnGsdkHealthCheck;
        OnGsdkHealthCheck.BindDynamic(this, &UMyGameInstance::OnGSDKHealthCheck);
        FOnGSDKServerActive_Dyn OnGSDKServerActive;
        OnGSDKServerActive.BindDynamic(this, &UMyGameInstance::OnGSDKServerActive);
        FOnGSDKReadyForPlayers_Dyn OnGSDKReadyForPlayers;
        OnGSDKReadyForPlayers.BindDynamic(this, &UMyGameInstance::OnGSDKReadyForPlayers);

        UGSDKUtils::RegisterGSDKShutdownDelegate(OnGsdkShutdown);
        UGSDKUtils::RegisterGSDKHealthCheckDelegate(OnGsdkHealthCheck);
        UGSDKUtils::RegisterGSDKServerActiveDelegate(OnGSDKServerActive);
        UGSDKUtils::RegisterGSDKReadyForPlayers(OnGSDKReadyForPlayers);
    }

#if UE_SERVER
    UGSDKUtils::SetDefaultServerHostPort();
#endif
}

void UMyGameInstance::OnStart()
{
    UE_LOG(LogPlayFabGSDKGameInstance, Warning, TEXT("Reached onStart!"));
    UGSDKUtils::ReadyForPlayers();
}

void UMyGameInstance::OnGSDKShutdown()
{
    UE_LOG(LogPlayFabGSDKGameInstance, Warning, TEXT("Shutdown!"));
    FPlatformMisc::RequestExit(false);
}

bool UMyGameInstance::OnGSDKHealthCheck()
{
    return true;
}

void UMyGameInstance::OnGSDKServerActive()
{
    UE_LOG(LogPlayFabGSDKGameInstance, Warning, TEXT("Active!"));
}

void UMyGameInstance::OnGSDKReadyForPlayers()
{
    UE_LOG(LogPlayFabGSDKGameInstance, Warning, TEXT("Finished Initialization - Moving to StandBy!"));
}

