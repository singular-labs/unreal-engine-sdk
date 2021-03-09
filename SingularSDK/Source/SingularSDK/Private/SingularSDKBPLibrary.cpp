// Copyright © 2010-2020 Singular Inc. All rights reserved.

#include "SingularSDKBPLibrary.h"
#include "SingularSDK.h"
#include "SingularDelegates.h"

#include "Logging/LogMacros.h"

#if PLATFORM_ANDROID
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#elif PLATFORM_IOS
#import <Singular/Singular.h>
#import <Singular/SingularConfig.h>
#endif

#define UNREAL_ENGINE_SDK_NAME "UnrealEngine"
#define UNREAL_ENGINE_SDK_VERSION "1.3.0"

#if PLATFORM_ANDROID

// FJavaWrapper JNI args mapping
// Bool     Z
// Int      I
// Double   D
// FString  Ljava/lang/String
// Void     V
// int[]    [I
// double[] [D

jobject TmapToJNIMap(TMap<FString, FString> tmap)
{
    JNIEnv* env = FAndroidApplication::GetJavaEnv();
    jclass mapClass = env->FindClass("java/util/HashMap");
    jmethodID mapConstructor = env->GetMethodID(mapClass, "<init>", "()V");
    jobject map = env->NewObject(mapClass, mapConstructor);
    jmethodID putMethod = env->GetMethodID(mapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    
    for (const TPair<FString, FString>& pair : tmap) {
        env->CallObjectMethod(map, putMethod, env->NewStringUTF(TCHAR_TO_UTF8(*pair.Key)), env->NewStringUTF(TCHAR_TO_UTF8(*pair.Value)));
    }
    
    return map;
}

#elif PLATFORM_IOS

NSDictionary* TmapToNSDictionary(TMap<FString, FString> tmap)
{
    NSMutableDictionary* data = [NSMutableDictionary new];
    for (TPair<FString, FString> pair : tmap) {
        [data setObject:pair.Value.GetNSString() forKey:pair.Key.GetNSString()];
    }
    return data;
}

void BroadcastConversionValueUpdated(int conversionValue) {
    for (TObjectIterator<USingularDelegates> Itr; Itr; ++Itr) {
        Itr->OnConversionValueUpdated.Broadcast(conversionValue);
    }
}

#endif

USingularSDKBPLibrary::USingularSDKBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
    
}

bool USingularSDKBPLibrary::isInitialized = false;

bool USingularSDKBPLibrary::Initialize(FString apiKey, FString apiSecret,
                                       int sessionTimeout,
                                       FString customUserId,
                                       bool skAdNetworkEnabled,
                                       bool manualSkanConversionManagement,
                                       int waitForTrackingAuthorizationWithTimeoutInterval,
                                       bool oaidCollection)
{
#if PLATFORM_ANDROID
    JNIEnv* env = FAndroidApplication::GetJavaEnv();
    jmethodID setSingularWrapper = FJavaWrapper::FindMethod(env, FJavaWrapper::GameActivityClassID, "setSingularWrapper", "(Ljava/lang/String;Ljava/lang/String;)V", false);
    FJavaWrapper::CallVoidMethod(env, FJavaWrapper::GameActivityThis, setSingularWrapper, env->NewStringUTF(UNREAL_ENGINE_SDK_NAME), env->NewStringUTF(UNREAL_ENGINE_SDK_VERSION));
    
    jmethodID setSingularData = FJavaWrapper::FindMethod(env, FJavaWrapper::GameActivityClassID, "setSingularData", "(Ljava/util/Map;)V", false);
    
    TMap<FString, FString> configValues;
    configValues.Add(TEXT("apikey"), apiKey);
    configValues.Add(TEXT("secret"), apiSecret);
    configValues.Add(TEXT("sessionTimeout"), FString::FromInt(sessionTimeout));
    configValues.Add(TEXT("oaidCollection"), oaidCollection ? TEXT("true") : TEXT("false"));
    
    if (customUserId.Len() > 0) {
        configValues.Add(TEXT("customUserId"), customUserId);
    }
    
    FJavaWrapper::CallVoidMethod(env, FJavaWrapper::GameActivityThis, setSingularData, TmapToJNIMap(configValues));
    
#elif PLATFORM_IOS
    NSString* key = apiKey.GetNSString();
    NSString* secret = apiSecret.GetNSString();
    
    SingularConfig* singularConfig = [[SingularConfig alloc] initWithApiKey:key andSecret:secret];
    singularConfig.skAdNetworkEnabled = skAdNetworkEnabled;
    singularConfig.manualSkanConversionManagement = manualSkanConversionManagement;
    singularConfig.waitForTrackingAuthorizationWithTimeoutInterval = waitForTrackingAuthorizationWithTimeoutInterval;
    
    if (singularConfig.skAdNetworkEnabled) {
        singularConfig.conversionValueUpdatedCallback = ^(NSInteger conversionValue) {
            BroadcastConversionValueUpdated(conversionValue);
        };
    }
    
    if (customUserId.Len() > 0) {
        [Singular setCustomUserId:customUserId.GetNSString()];
    }
    
    [Singular setSessionTimeout:sessionTimeout];
    [Singular setWrapperName:@UNREAL_ENGINE_SDK_NAME andVersion:@UNREAL_ENGINE_SDK_VERSION];
    [Singular start:singularConfig];
#endif
    
    // Unreal Engine uses execptions disabled flag we can't add try/catch
    isInitialized = true;
    
    return isInitialized;
}

void USingularSDKBPLibrary::SendEvent(FString eventName)
{
    if (!isInitialized) {
        return;
    }
    
#if PLATFORM_ANDROID
    JNIEnv* env = FAndroidApplication::GetJavaEnv();
    jmethodID singularEvent = FJavaWrapper::FindMethod(env, FJavaWrapper::GameActivityClassID, "singularEvent", "(Ljava/lang/String;)V", false);
    jstring name = env->NewStringUTF(TCHAR_TO_UTF8(*eventName));
    FJavaWrapper::CallVoidMethod(env, FJavaWrapper::GameActivityThis, singularEvent, name);
#elif PLATFORM_IOS
    [Singular event:eventName.GetNSString()];
#endif
}

void USingularSDKBPLibrary::SendEventWithArgs(FString eventName, TMap<FString, FString>args)
{
    if (!isInitialized) {
        return;
    }
    
#if PLATFORM_ANDROID
    JNIEnv* env = FAndroidApplication::GetJavaEnv();
    jmethodID singularEventWithArgs = FJavaWrapper::FindMethod(env, FJavaWrapper::GameActivityClassID, "singularEventWithArgs", "(Ljava/lang/String;Ljava/util/Map;)V", false);
    jstring name = env->NewStringUTF(TCHAR_TO_UTF8(*eventName));
    jobject map = TmapToJNIMap(args);
    FJavaWrapper::CallVoidMethod(env, FJavaWrapper::GameActivityThis, singularEventWithArgs, name, map);
#elif PLATFORM_IOS
    [Singular event:eventName.GetNSString() withArgs:TmapToNSDictionary(args)];
#endif
}

void USingularSDKBPLibrary::SendRevenue(FString eventName, FString currency, float amount)
{
    if (!isInitialized) {
        return;
    }
    
#if PLATFORM_ANDROID
    JNIEnv* env = FAndroidApplication::GetJavaEnv();
    jmethodID singularRevenue = FJavaWrapper::FindMethod(env, FJavaWrapper::GameActivityClassID, "singularRevenue", "(Ljava/lang/String;Ljava/lang/String;D)V", false);
    jstring name = env->NewStringUTF(TCHAR_TO_UTF8(*eventName));
    jstring cur = env->NewStringUTF(TCHAR_TO_UTF8(*currency));
    FJavaWrapper::CallVoidMethod(env, FJavaWrapper::GameActivityThis, singularRevenue, name, cur, amount);
#elif PLATFORM_IOS
    [Singular customRevenue:eventName.GetNSString() currency:currency.GetNSString() amount:amount];
#endif
}

void USingularSDKBPLibrary::SendRevenueWithArgs(FString eventName, FString currency, float amount, TMap<FString, FString> args)
{
    if (!isInitialized) {
        return;
    }
    
#if PLATFORM_ANDROID
    JNIEnv* env = FAndroidApplication::GetJavaEnv();
    jmethodID singularRevenueWithArgs = FJavaWrapper::FindMethod(env, FJavaWrapper::GameActivityClassID, "singularRevenueWithArgs", "(Ljava/lang/String;Ljava/lang/String;DLjava/util/Map;)V", false);
    jstring name = env->NewStringUTF(TCHAR_TO_UTF8(*eventName));
    jstring cur = env->NewStringUTF(TCHAR_TO_UTF8(*currency));
    jobject map = TmapToJNIMap(args);
    FJavaWrapper::CallVoidMethod(env, FJavaWrapper::GameActivityThis, singularRevenueWithArgs, name, cur, amount, map);
#elif PLATFORM_IOS
    [Singular customRevenue:eventName.GetNSString() currency:currency.GetNSString() amount:amount withAttributes:TmapToNSDictionary(args)];
#endif
}

void USingularSDKBPLibrary::SetCustomUserId(FString customUserId)
{
    if (!isInitialized) {
        return;
    }
    
#if PLATFORM_ANDROID
    JNIEnv* env = FAndroidApplication::GetJavaEnv();
    jmethodID singularSetCustomUserId = FJavaWrapper::FindMethod(env, FJavaWrapper::GameActivityClassID, "singularSetCustomUserId", "(Ljava/lang/String;)V", false);
    jstring userId = env->NewStringUTF(TCHAR_TO_UTF8(*customUserId));
    FJavaWrapper::CallVoidMethod(env, FJavaWrapper::GameActivityThis, singularSetCustomUserId, userId);
#elif PLATFORM_IOS
    [Singular setCustomUserId:customUserId.GetNSString()];
#endif
}

void USingularSDKBPLibrary::UnsetCustomUserId()
{
    if (!isInitialized) {
        return;
    }
    
#if PLATFORM_ANDROID
    JNIEnv* env = FAndroidApplication::GetJavaEnv();
    jmethodID singularUnsetCustomUserId = FJavaWrapper::FindMethod(env, FJavaWrapper::GameActivityClassID, "singularUnsetCustomUserId", "()V", false);
    FJavaWrapper::CallVoidMethod(env, FJavaWrapper::GameActivityThis, singularUnsetCustomUserId);
#elif PLATFORM_IOS
    [Singular unsetCustomUserId];
#endif
}

void USingularSDKBPLibrary::SetUninstallToken(FString token)
{
    if (!isInitialized) {
        return;
    }
    
#if PLATFORM_ANDROID
    JNIEnv* env = FAndroidApplication::GetJavaEnv();
    jmethodID singularSetUninstallToken = FJavaWrapper::FindMethod(env, FJavaWrapper::GameActivityClassID, "singularSetUninstallToken", "(Ljava/lang/String;)V", false);
    jstring uninstallToken = env->NewStringUTF(TCHAR_TO_UTF8(*token));
    FJavaWrapper::CallVoidMethod(env, FJavaWrapper::GameActivityThis, singularSetUninstallToken, uninstallToken);
#elif PLATFORM_IOS
    [Singular registerDeviceTokenForUninstall:[token.GetNSString() dataUsingEncoding:NSUTF8StringEncoding]];
#endif
}

/* GDPR helpers */

void USingularSDKBPLibrary::TrackingOptIn()
{
    if (!isInitialized) {
        return;
    }
    
#if PLATFORM_ANDROID
    JNIEnv* env = FAndroidApplication::GetJavaEnv();
    jmethodID singularTrackingOptIn = FJavaWrapper::FindMethod(env, FJavaWrapper::GameActivityClassID, "singularTrackingOptIn", "()V", false);
    FJavaWrapper::CallVoidMethod(env, FJavaWrapper::GameActivityThis, singularTrackingOptIn);
#elif PLATFORM_IOS
    [Singular trackingOptIn];
#endif
}

void USingularSDKBPLibrary::TrackingUnder13()
{
    if (!isInitialized) {
        return;
    }
    
#if PLATFORM_ANDROID
    JNIEnv* env = FAndroidApplication::GetJavaEnv();
    jmethodID singularTrackingUnder13 = FJavaWrapper::FindMethod(env, FJavaWrapper::GameActivityClassID, "singularTrackingUnder13", "()V", false);
    FJavaWrapper::CallVoidMethod(env, FJavaWrapper::GameActivityThis, singularTrackingUnder13);
#elif PLATFORM_IOS
    [Singular trackingUnder13];
#endif
}

void USingularSDKBPLibrary::StopAllTracking()
{
    if (!isInitialized) {
        return;
    }
    
#if PLATFORM_ANDROID
    JNIEnv* env = FAndroidApplication::GetJavaEnv();
    jmethodID singularStopAllTracking = FJavaWrapper::FindMethod(env, FJavaWrapper::GameActivityClassID, "singularStopAllTracking", "()V", false);
    FJavaWrapper::CallVoidMethod(env, FJavaWrapper::GameActivityThis, singularStopAllTracking);
#elif PLATFORM_IOS
    [Singular stopAllTracking];
#endif
}

void USingularSDKBPLibrary::ResumeAllTracking()
{
    if (!isInitialized) {
        return;
    }
    
#if PLATFORM_ANDROID
    JNIEnv* env = FAndroidApplication::GetJavaEnv();
    jmethodID singularResumeAllTracking = FJavaWrapper::FindMethod(env, FJavaWrapper::GameActivityClassID, "singularResumeAllTracking", "()V", false);
    FJavaWrapper::CallVoidMethod(env, FJavaWrapper::GameActivityThis, singularResumeAllTracking);
#elif PLATFORM_IOS
    [Singular resumeAllTracking];
#endif
}

bool USingularSDKBPLibrary::IsAllTrackingStopped()
{
    if (!isInitialized) {
        return false;
    }
    
    bool isAllTrackingStopped = false;
#if PLATFORM_ANDROID
    JNIEnv* env = FAndroidApplication::GetJavaEnv();
    jmethodID singularIsAllTrackingStopped = FJavaWrapper::FindMethod(env, FJavaWrapper::GameActivityClassID, "singularIsAllTrackingStopped", "()Z", false);
    isAllTrackingStopped = FJavaWrapper::CallBooleanMethod(env, FJavaWrapper::GameActivityThis, singularIsAllTrackingStopped);
#elif PLATFORM_IOS
    isAllTrackingStopped = [Singular isAllTrackingStopped];
#endif
    return isAllTrackingStopped;
}

void USingularSDKBPLibrary::LimitDataSharing(bool shouldLimitDataSharing)
{
    if (!isInitialized) {
        return;
    }
    
#if PLATFORM_ANDROID
    JNIEnv* env = FAndroidApplication::GetJavaEnv();
    jmethodID singularLimitDataSharing = FJavaWrapper::FindMethod(env, FJavaWrapper::GameActivityClassID, "singularLimitDataSharing", "(Z)V", false);
    FJavaWrapper::CallVoidMethod(env, FJavaWrapper::GameActivityThis, singularLimitDataSharing, shouldLimitDataSharing);
#elif PLATFORM_IOS
    [Singular limitDataSharing:shouldLimitDataSharing];
#endif
}

bool USingularSDKBPLibrary::GetLimitDataSharing()
{
    if (!isInitialized) {
        return false;
    }
    
    bool limitDataSharing = false;
#if PLATFORM_ANDROID
    JNIEnv* env = FAndroidApplication::GetJavaEnv();
    jmethodID singularGetLimitDataSharing = FJavaWrapper::FindMethod(env, FJavaWrapper::GameActivityClassID, "singularGetLimitDataSharing", "()Z", false);
    limitDataSharing = FJavaWrapper::CallBooleanMethod(env, FJavaWrapper::GameActivityThis, singularGetLimitDataSharing);
#elif PLATFORM_IOS
    limitDataSharing = [Singular getLimitDataSharing];
#endif
    return limitDataSharing;
}

/* SKAN Methods */

bool USingularSDKBPLibrary::SkanUpdateConversionValue(int conversionValue)
{
#if PLATFORM_ANDROID
#elif PLATFORM_IOS
    NSInteger nsiConversionValue = (NSInteger)conversionValue;
    return [Singular skanUpdateConversionValue:nsiConversionValue];
#endif
    
    return true;
}

int USingularSDKBPLibrary::SkanGetConversionValue()
{
    int conversionValue = 0;
#if PLATFORM_ANDROID
#elif PLATFORM_IOS
    conversionValue = [[Singular skanGetConversionValue] intValue];
#endif
    return conversionValue;
}

void USingularSDKBPLibrary::SkanRegisterAppForAdNetworkAttribution()
{
#if PLATFORM_ANDROID
#elif PLATFORM_IOS
    [Singular skanRegisterAppForAdNetworkAttribution];
#endif
}
