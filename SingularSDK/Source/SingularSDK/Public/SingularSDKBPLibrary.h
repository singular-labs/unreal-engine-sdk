// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SingularSDKBPLibrary.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
DECLARE_LOG_CATEGORY_EXTERN(LogSingularSDKBlueprint, Display, Display);

UCLASS()
class USingularSDKBPLibrary : public UBlueprintFunctionLibrary
{
    static bool isInitialized;
    
    GENERATED_UCLASS_BODY()

    // start
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Initialize Singular SDK", Keywords = "Singular"), Category = "Singular-SDK")
    static bool Initialize(FString apiKey, FString apiSecret, bool skAdNetworkEnabled=false);
    
    // track events
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Send Event", Keywords = "Singular Event"), Category = "Singular-SDK")
    static void SendEvent(FString eventName);
    
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Send Event with args", Keywords = "Singular Event"), Category = "Singular-SDK")
    static void SendEventWithArgs(FString eventName, TMap<FString, FString> args);
    
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Custom User Id", Keywords = "Custom User Id"), Category = "Singular-SDK")
    static void SetCustomUserId(FString customUserId);
    
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Unset Custom User Id", Keywords = "Custom User Id"), Category = "Singular-SDK")
    static void UnsetCustomUserId();
    
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Revenue", Keywords = "revenue"), Category = "Singular-SDK")
    static void Revenue(FString currency, float amount);
    
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
};
