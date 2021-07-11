// Copyright © 2010-2020 Singular Inc. All rights reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SingularSDKBPLibrary.generated.h"

UCLASS()
class SINGULARSDK_API USingularSDKBPLibrary : public UBlueprintFunctionLibrary
{
    static bool isInitialized;
    static FString singularApiKey;
    static FString singularSecret;
    static int singularSessionTimeout;
    static bool singularSkanEnabled;
    static bool singularManualSkanConversionManagement;
    static int singularWaitForTrackingAuthorizationWithTimeoutInterval;
    
    static bool didRegisterToOpenUrl;
    static bool didRegisterToWillGoToBackground;
    
    GENERATED_UCLASS_BODY()
    static void configure();

    // start
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Initialize Singular SDK", Keywords = "Singular"), Category = "Singular-SDK")
    static bool Initialize(FString apiKey, FString apiSecret,
                           int sessionTimeout = 60,
                           FString customUserId = TEXT(""),
                           bool skAdNetworkEnabled = false,
                           bool manualSkanConversionManagement = false,
                           int waitForTrackingAuthorizationWithTimeoutInterval = 0,
                           bool oaidCollection = false);
    
    // track events
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Send Event", Keywords = "Singular Event"), Category = "Singular-SDK")
    static void SendEvent(FString eventName);
    
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Send Event With Args", Keywords = "Singular Event"), Category = "Singular-SDK")
    static void SendEventWithArgs(FString eventName, TMap<FString, FString> args);
    
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Send Revenue", Keywords = "Singular Revenue"), Category = "Singular-SDK")
    static void SendRevenue(FString eventName, FString currency, float amount);
    
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Send Revenue With Args", Keywords = "Singular Revenue"), Category = "Singular-SDK")
    static void SendRevenueWithArgs(FString eventName, FString currency, float amount, TMap<FString, FString> args);
    
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Custom User Id", Keywords = "Custom User Id"), Category = "Singular-SDK")
    static void SetCustomUserId(FString customUserId);
    
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Unset Custom User Id", Keywords = "Custom User Id"), Category = "Singular-SDK")
    static void UnsetCustomUserId();
    
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Uninstall Token", Keywords = "Uninstall"), Category = "Singular-SDK")
    static void SetUninstallToken(FString token);
    
    /* GDPR helpers */
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Tracking Opt In", Keywords = "GDPR"), Category = "Singular-SDK")
    static void TrackingOptIn();
    
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Tracking Under 13", Keywords = "GDPR"), Category = "Singular-SDK")
    static void TrackingUnder13();
    
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Stop All Tracking", Keywords = "GDPR"), Category = "Singular-SDK")
    static void StopAllTracking();
    
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Resume All Tracking", Keywords = "GDPR"), Category = "Singular-SDK")
    static void ResumeAllTracking();
    
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Is All Tracking Stopped", Keywords = "GDPR"), Category = "Singular-SDK")
    static bool IsAllTrackingStopped();
    
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Limit Data Sharing", Keywords = "CCPA"), Category = "Singular-SDK")
    static void LimitDataSharing(bool shouldLimitDataSharing);
    
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Limit Data Sharing", Keywords = "CCPA"), Category = "Singular-SDK")
    static bool GetLimitDataSharing();

    /* SKAN Methods */
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "SKAN Update Conversion Value", Keywords = "SKAN"), Category = "Singular-SDK")
    static bool SkanUpdateConversionValue(int conversionValue);
    
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "SKAN Get Conversion Value", Keywords = "SKAN"), Category = "Singular-SDK")
    static int SkanGetConversionValue();
    
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "SKAN Register App For AdNetwork Attribution", Keywords = "SKAN"), Category = "Singular-SDK")
    static void SkanRegisterAppForAdNetworkAttribution();
    
private:
#if PLATFORM_IOS
    static void InitializeIOS(NSDictionary* launchOptions, NSURL *url);
    static void OnOpenURL(UIApplication *application, NSURL *url, NSString *sourceApplication, id annotation);
    static void OnWillResignActive();
#endif
};
