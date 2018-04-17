// Copyright 2018 Pavlov Dmitriy

using UnrealBuildTool;
using System.Collections.Generic;

public class LevelGenTarget : TargetRules
{
	public LevelGenTarget(TargetInfo Target) : base(Target)
    {
		Type = TargetType.Game;
        ExtraModuleNames.AddRange(new string[] { "LevelGen" });
    }

	//
	// TargetRules interface.
	//

	
}
