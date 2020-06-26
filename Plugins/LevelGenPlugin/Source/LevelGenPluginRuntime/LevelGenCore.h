// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "CoreMinimal.h"
#include <memory>
#include <map>
#include <vector>
#include <tuple>
#include <utility>
#include <functional>
#include <random>   
#include "UObject/Class.h"  //For UENUM

struct FLevelGeneratorSettings;

enum class EYawTurn : uint8
{
	Zero,
	HalfPi,
	Pi,
	PiAndHalf
};

int EYawTurnGetAngle(EYawTurn YawTurn);




UENUM()
enum class EDirection : uint8
{
	XP,
	XM,
	YP,
	YM,
	end
};

inline EDirection EDirectionGetRight(EDirection Direction);

inline EDirection EDirectionGetLeft(EDirection Direction);

inline FVector EDirectionGetVector(EDirection Direction);

inline FVector2D EDirectionGetVector2D(EDirection Direction);

inline EDirection EDirectionFromVector(FVector Vector);

inline EDirection EDirectionInvert(EDirection Direction);


enum class EWallCornerType : uint8
{
	NegativeOffset,
	Normal,
	NormalWithEdge,
	PositiveOffset,
	end
};

struct FVectorLess

{
	bool operator()(const FVector& Ar1, const FVector& Ar2) const
	{
		if (Ar1.X != Ar2.X)
		{
			return Ar1.X < Ar2.X;
		}
		else if (Ar1.Y != Ar2.Y)
		{
			return Ar1.Y < Ar2.Y;
		}
		else if (Ar1.Z != Ar2.Z)
		{
			return Ar1.Z < Ar2.Z;
		}
		return false;
	}
};


struct FVector2DLess
{
	bool operator()(const FVector2D& Ar1, const FVector2D& Ar2) const
	{
		if (Ar1.X != Ar2.X)
		{
			return Ar1.X < Ar2.X;
		}
		else if (Ar1.Y != Ar2.Y)
		{
			return Ar1.Y < Ar2.Y;
		}
		return false;
	}

};




template<typename T>
extern bool ComparateProperty(UScriptStruct* Struct, const T& left, const T& right);

enum class EQuarterPlane : uint8
{
	PlusPlus,
	PlusMinus,
	MinusMinus,
	MinusPlus
};

enum class ESemiAxis
{
	Plus,
	Minus
};
template <typename T>
class TLevelMapBase
{
public:
	void DestroyMap();
protected:
	std::vector<T> MapCellData;
};

template <typename T>
class TLevelMap1D : public TLevelMapBase<std::vector<std::unique_ptr<T>>>
{
public:

	TLevelMap1D() { MapCellData.resize(2); }

	std::unique_ptr<T>& GetCell(int X);

	const std::unique_ptr<T>& GetCell(int X) const;

	bool IsCellExist(int X) const;

	void FillRange(int Start, int End, const std::function<std::unique_ptr<T> (int X)> Filler);

	

private:

	std::tuple<int, ESemiAxis> GetInternalCellAddres(int X) const;

	void CheckArraySize(int X, ESemiAxis SemiAxis);

};


template <typename T>
class TLevelMap2D : public TLevelMapBase<std::vector<std::vector<std::unique_ptr<T>>>>
{

public:

	TLevelMap2D() { MapCellData.resize(4); }

	std::unique_ptr<T>& GetCell(int X, int Y);

	std::unique_ptr<T>& GetCell(FVector2D Coordinate) { return GetCell(Coordinate.X, Coordinate.Y); }

	const std::unique_ptr<T>& GetCell(int X, int Y) const;

	const std::unique_ptr<T>& GetCell(FVector2D Coordinate) const { return GetCell(Coordinate.X, Coordinate.Y); }

	bool IsCellExist(int X, int Y) const;

	bool IsCellExist(FVector2D Coordinate) const { return IsCellExist(Coordinate.X, Coordinate.Y); }

	void FillRange(FVector2D Start, FVector2D End, const std::function<std::unique_ptr<T> (int X, int Y)> Filler);

private:
	void ReservRange(FVector2D Start, FVector2D End);

	std::tuple<int, int, EQuarterPlane> GetInternalCellAddres(int X, int Y) const;

	void CheckArraySize(int X, int Y, EQuarterPlane QuarterPlane);

};



class FTexture2D;

template <typename T>
class TDataStorageFromTexture
{
public:
	TDataStorageFromTexture(FTexture2D* Texture, const std::function<T(FColor Color)> ColrToTypeConverter, std::mt19937& _RandomGenerator);

	T GetDataFromCoordinate(int X, int Y);

private:
	std::pair<int, int> RecountCoordinateForMapSize(int X, int Y);

	static void GetDataFromTexture(std::vector<std::vector<FColor>>& TextureData, FTexture2D* Texture);

	std::pair<int, int> GetRandomStartOffsetForMap();

private:
	std::mt19937& RandomGenerator;

	std::vector<std::vector<T>> Data;

	std::pair<int, int> MapOffset;
};

class UObjectsDisposer
{

public:

	UObjectsDisposer(std::pair<FVector2D, FVector2D> _SearchAreaRange, std::mt19937& _RandomGenerator, const FLevelGeneratorSettings& _LevelSettings) :
		LevelSettings(_LevelSettings), SearchAreaRange(_SearchAreaRange), RandomGenerator(_RandomGenerator)
	{
		if (SearchAreaRange.first.X > SearchAreaRange.second.X || SearchAreaRange.first.Y > SearchAreaRange.second.Y) throw FString("Bad search area");
	}

	bool GetLocation(FVector2D& Location, FVector2D SearchStartCoordinate, int FloorForSearch, FVector2D SearchObjectSize, std::function<bool(FVector)>IsLocationFitPredicate) const;

	bool GetLocation(FVector& Location, FVector2D SearchStartCoordinate, FVector2D SearchObjectSize, std::function<bool(FVector)>IsLocationFitPredicate) const;

	FVector2D GetSearchAreaSize() const { return SearchAreaRange.second - SearchAreaRange.first + FVector2D(1, 1); }

private:

	void GetRectangleCoordinate(std::vector<FVector2D>& Coordinats, FVector2D Center, int Radius) const;

	void GetXPlus(std::vector<FVector2D>& Coordinats, FVector2D Center, int Radius) const;

	void GetYPlus(std::vector<FVector2D>& Coordinats, FVector2D Center, int Radius) const;

	void GetXMinus(std::vector<FVector2D>& Coordinats, FVector2D Center, int Radius) const;

	void GetYMinus(std::vector<FVector2D>& Coordinats, FVector2D Center, int Radius) const;

	int GetRandomNumber(int From, int To) const;

private:

	const FLevelGeneratorSettings& LevelSettings;

	std::pair<FVector2D, FVector2D> SearchAreaRange;

	std::mt19937& RandomGenerator;
};




template<typename TEnum>
static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
	if (!enumPtr)
	{
		return FString("Invalid");
	}

	return enumPtr->GetNameStringByIndex((int32)Value);
}

template<typename TEnum>
static FORCEINLINE TEnum GetEnumValueFromString(const FString& NameOfClass, const FString& ValueName)
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, *NameOfClass, true);
	if (!EnumPtr) return (TEnum)0;

	return (TEnum)EnumPtr->GetIndexByName(FName(*ValueName));
}



enum class ERoundType
{
	Down,
	Up,
	Mathematical
};


inline float RoundNumber(float Number, ERoundType RoundType = ERoundType::Down)
{
	switch (RoundType)
	{
	case ERoundType::Up:

		return Number < 0 ? floor(Number) : ceil(Number);

	case ERoundType::Down:

		return Number < 0 ? ceil(Number) : floor(Number);

	case ERoundType::Mathematical:

		return Number < 0 ? ceil(Number - 0.5) : floor(Number + 0.5);

	default:

		throw;

	}
	
}

inline bool IsNumberEven(int Number)
{
	return Number & 1 ? false : true;
}



inline bool GetIntervalIntesect(std::pair<int, int>& IntersecInterval, std::pair<int, int> FirstInterval, std::pair<int, int>SecondInterval);

 std::pair<int, int> GetCircleCoordinateFromX(int X, FVector2D CenterCoordinate, int Radius);

bool IsPointInCircle(int X, int Y, FVector2D CenterCoordinate, int Radius);