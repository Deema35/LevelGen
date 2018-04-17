// Copyright 2018 Pavlov Dmitriy

using UnrealBuildTool;
using System.Collections.Generic;

public class LevelGenEditorTarget : TargetRules
{
	public LevelGenEditorTarget(TargetInfo Target) : base(Target)
    {
		Type = TargetType.Editor;
        ExtraModuleNames.AddRange(new string[] { "LevelGen" });
    }

	//
	// TargetRules interface.
	//

}
