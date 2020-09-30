// Copyright © 2010-2020 Singular Inc. All rights reserved.

using UnrealBuildTool;

public class SingularSDK : ModuleRules
{
    public SingularSDK(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
                // ... add public include paths required here ...
            }
            );
                
        PrivateIncludePaths.AddRange(
            new string[] {
                // ... add other private include paths required here ...
            }
            );
                
            
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                // ... add other public dependencies that you statically link with here ...
            }
            );
            
        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                // ... add private dependencies that you statically link with here ...
            }
            );
        
        
        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
                // ... add any modules that your module loads dynamically here ...
            }
            );
   
        if (Target.Platform == UnrealTargetPlatform.IOS)
        {
            PublicAdditionalLibraries.Add("sqlite3.0");
            PublicAdditionalLibraries.Add("z");
            PublicAdditionalFrameworks.Add(
               new Framework(
                   "Singular",
                   "Singular.framework.10.1.2.zip"
               )
            );
            PublicFrameworks.AddRange(new string[] { "AdSupport", "iAd", "StoreKit", "SystemConfiguration", "Security"});
        } else if (Target.Platform == UnrealTargetPlatform.Android)
        {
            // Unreal Plugin Language
            string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", System.IO.Path.Combine(PluginPath, "Singular_UPL.xml"));
            // JNI
            PublicIncludePathModuleNames.Add("Launch");
        }
    }
}
