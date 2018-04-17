// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "CoreMinimal.h"
#include <vector>
#include <memory>

class AVirtualSpawner;
class FDataFillerBase;
class FLevelBilderBase;
enum class EDataFiller : uint8;
enum class ELevelBilder : uint8;
struct FGenerateOrderModule;
struct FDataStorage;
struct FLevelGeneratorSettings;
class ALevelGenerator;

class FGenerateObject
{
public:
	virtual ~FGenerateObject() {};

	virtual int GetDeltaOffsetGenerateArea() const { return 0; }

	virtual FString GetModuleName() const = 0;

	virtual bool IsServiceObject() { return false; }

	virtual void Generate(FVector2D CenterCoordinate, int Radius, AVirtualSpawner* VirtualSpawner) = 0;

};

class FGenerateObjectBlanc : public FGenerateObject
{
public:
	FGenerateObjectBlanc(int _Delta) : Delta(_Delta) {}

	virtual int GetDeltaOffsetGenerateArea() const override { return Delta; }

	virtual void Generate(FVector2D CenterCoordinate, int Radius, AVirtualSpawner* VirtualSpawner) override {}

	virtual FString GetModuleName() const override { return FString("Blank object"); }

	virtual bool IsServiceObject() { return true; }

private:

	int Delta;
};



class FGenerateMashine
{
public:
	FGenerateMashine(TArray<std::unique_ptr<FGenerateOrderModule>>& GenerateOrder, FDataStorage& DataStorage,
		const FLevelGeneratorSettings& LevelSettings, const ALevelGenerator& LevelGenerator);

	
	int GetRadiusNecessaryForGenerate();

	void Generate(FVector2D GenerateCenter, AVirtualSpawner* VirtualSpawner);

	void SetShowModuleLeadTime(bool _ShowModuleLeadTime) { ShowModuleLeadTime = _ShowModuleLeadTime; }


private:

	void AddGenerateObject(FGenerateObject* GenerateObjects);

	void AddBlancObject(int  Delta);

	bool ShowModuleLeadTime = false;

	std::vector<std::unique_ptr<FGenerateObject>> GenerateObjects;
};
