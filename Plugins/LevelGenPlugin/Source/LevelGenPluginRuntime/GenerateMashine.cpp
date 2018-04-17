// Copyright 2018 Pavlov Dmitriy
#include"GenerateMashine.h"
#include <chrono>
#include "LevelGenPluginRuntime_LogCategory.h"
#include "LevelGenerator.h"


//.........................................
//FGenerateMashine
//.........................................

FGenerateMashine::FGenerateMashine(TArray<std::unique_ptr<FGenerateOrderModule>>& GenerateOrder, FDataStorage& DataStorage,
	const FLevelGeneratorSettings& LevelSettings, const ALevelGenerator& LevelGenerator)
{

	for (int i = 0; i < GenerateOrder.Num(); i++)
	{
		std::vector<EDataFiller> CurrentDataFillers;
		std::vector<ELevelBilder> CurrentLevelBilders;

		GenerateOrder[i]->GetGenerateObject(CurrentDataFillers);
		for (int j = 0; j < CurrentDataFillers.size(); j++)
		{
			AddGenerateObject(EDataFillerCreate(CurrentDataFillers[j], DataStorage, LevelSettings, LevelGenerator));
			
		}

		GenerateOrder[i]->GetGenerateObject(CurrentLevelBilders);

		for (int j = 0; j < CurrentLevelBilders.size(); j++)
		{
			AddGenerateObject(ELevelBilderCreate(CurrentLevelBilders[j], DataStorage, LevelSettings, LevelGenerator));
			
		}

		if (GenerateOrder[i]->AddedGenerateRange > 0)
		{
			AddBlancObject(GenerateOrder[i]->AddedGenerateRange);
		}

	}

}

void FGenerateMashine::AddGenerateObject(FGenerateObject* GenerateObject)
{
	GenerateObjects.push_back(std::unique_ptr<FGenerateObject>(GenerateObject));

}


int FGenerateMashine::GetRadiusNecessaryForGenerate()
{

	int Radius = 0;

	for (int i = 0; i < GenerateObjects.size(); i++)
	{
		
		Radius = Radius + GenerateObjects[i]->GetDeltaOffsetGenerateArea();
		
	}

	return Radius;
}

void FGenerateMashine::Generate(FVector2D GenerateCenter, AVirtualSpawner* VirtualSpawner)
{

	int GenerateRadius = GetRadiusNecessaryForGenerate();

	for (int i = 0; i < GenerateObjects.size(); i++)
	{
		

		GenerateRadius = GenerateRadius - GenerateObjects[i]->GetDeltaOffsetGenerateArea();

		
		if (ShowModuleLeadTime && !GenerateObjects[i]->IsServiceObject())
		{
			auto BeginGenerateTime = std::chrono::high_resolution_clock::now();

			GenerateObjects[i]->Generate(GenerateCenter, GenerateRadius, VirtualSpawner);

			auto EndGenerateTime = std::chrono::high_resolution_clock::now();
			int TimeMs = std::chrono::duration_cast<std::chrono::microseconds>(EndGenerateTime - BeginGenerateTime).count();
			float TimeS = (float)TimeMs / 1000000;
			UE_LOG(LogLevelGen, Display, TEXT("Generate Time module %s : %f s"), *GenerateObjects[i]->GetModuleName(), TimeS);
		}
		else
		{
			GenerateObjects[i]->Generate(GenerateCenter, GenerateRadius, VirtualSpawner);
		}
		

	}

}

void FGenerateMashine::AddBlancObject(int  Delta)
{

	AddGenerateObject(new FGenerateObjectBlanc(Delta));
}

