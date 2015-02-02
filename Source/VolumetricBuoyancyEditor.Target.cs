// Implementation created by David 'vebski' Niemiec

using UnrealBuildTool;
using System.Collections.Generic;

public class VolumetricBuoyancyEditorTarget : TargetRules
{
	public VolumetricBuoyancyEditorTarget(TargetInfo Target)
	{
		Type = TargetType.Editor;
	}

	//
	// TargetRules interface.
	//

	public override void SetupBinaries(
		TargetInfo Target,
		ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
		ref List<string> OutExtraModuleNames
		)
	{
		OutExtraModuleNames.AddRange( new string[] { "VolumetricBuoyancy" } );
	}
}
