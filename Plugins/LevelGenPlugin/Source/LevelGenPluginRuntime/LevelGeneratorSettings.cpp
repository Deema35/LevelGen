// Copyright 2018 Pavlov Dmitriy
#include "LevelGeneratorSettings.h"
#include "LevelGenCore.h"
#include "LevelGenPluginRuntime_LogCategory.h"


void FGenerateOrderModule::AddGenerateObject(EDataFiller GenerateObjectType)
{
	DataFillerObjects.Add(GenerateObjectType);
}

void FGenerateOrderModule::AddGenerateObject(ELevelBilder GenerateObjectType)
{
	LevelBilderObjects.Add(GenerateObjectType);
}

bool FLevelGeneratorMaterialSettings::operator== (const FLevelGeneratorMaterialSettings& right) const
{

	if (ComparateProperty<FLevelGeneratorMaterialSettings>(FLevelGeneratorMaterialSettings::StaticStruct(), *this, right))
	{
		
		return true;
	}

	return false;
}

//*******************************************************************
//FLevelGeneratorSettings
//*******************************************************************

FLevelGeneratorSettings::FLevelGeneratorSettings()
{
	HoverCarTrackLayers.SetNum(4);

	HoverCarTrackLayers[0].TrackHeight = 3600;
	HoverCarTrackLayers[0].TrackDirection = EHoverCarTrackDirection::Pozitive;
	HoverCarTrackLayers[0].OneHowerCarsOnRoadUnits = 4000;

	HoverCarTrackLayers[1].TrackHeight = 3400;
	HoverCarTrackLayers[1].TrackDirection = EHoverCarTrackDirection::Negotive;
	HoverCarTrackLayers[1].OneHowerCarsOnRoadUnits = 4000;

	HoverCarTrackLayers[2].TrackHeight = -3400;
	HoverCarTrackLayers[2].TrackDirection = EHoverCarTrackDirection::Negotive;
	HoverCarTrackLayers[2].OneHowerCarsOnRoadUnits = 4000;

	HoverCarTrackLayers[3].TrackHeight = -3500;
	HoverCarTrackLayers[3].TrackDirection = EHoverCarTrackDirection::Pozitive;
	HoverCarTrackLayers[3].OneHowerCarsOnRoadUnits = 4000;
}


bool FLevelGeneratorSettings::operator== (const FLevelGeneratorSettings& right) const
{
	
	if (ComparateProperty<FLevelGeneratorSettings>(FLevelGeneratorSettings::StaticStruct(), *this, right))
	{
		for (TFieldIterator<UProperty> PropIt(FLevelGeneratorSettings::StaticStruct()); PropIt; ++PropIt)
		{
			if (UStructProperty* StructProp = Cast<UStructProperty>(*PropIt))
			{
				
				
			}
		}
		return true;
	}

	return false;
}