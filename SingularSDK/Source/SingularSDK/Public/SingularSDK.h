// Copyright © 2010-2020 Singular Inc. All rights reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FSingularSDKModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
