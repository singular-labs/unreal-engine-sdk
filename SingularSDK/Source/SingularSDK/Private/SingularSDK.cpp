// Copyright © 2010-2020 Singular Inc. All rights reserved.

#include "SingularSDK.h"

#define LOCTEXT_NAMESPACE "FSingularSDKModule"

void FSingularSDKModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
}

void FSingularSDKModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSingularSDKModule, SingularSDK)