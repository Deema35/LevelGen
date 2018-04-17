// Copyright 2018 Pavlov Dmitriy
#include "LevelGenCore.h" 
#include "DrawDebugHelpers.h"
#include "LevelGeneratorSettings.h"
#include "LevelGenPluginRuntime_LogCategory.h"

//.........................................
//EYawTurn
//.........................................

int EYawTurnGetAngle(EYawTurn YawTurn)
{
	switch (YawTurn)
	{
	case EYawTurn::Zero: return 0;

	case EYawTurn::HalfPi: return 90;

	case EYawTurn::Pi: return 180;

	case EYawTurn::PiAndHalf: return 270;

	default: throw;

	}
}

//.........................................
//EDirection
//.........................................

EDirection EDirectionGetRight(EDirection Direction)
{
	switch (Direction)
	{
	case EDirection::XP: return EDirection::YP;

	case EDirection::YP: return EDirection::XM;

	case EDirection::XM: return EDirection::YM;

	case EDirection::YM: return EDirection::XP;

	default: throw;

	}
}

EDirection EDirectionGetLeft(EDirection Direction)
{
	switch (Direction)
	{
	case EDirection::XP: return EDirection::YM;

	case EDirection::YM: return EDirection::XM;

	case EDirection::XM: return EDirection::YP;

	case EDirection::YP: return EDirection::XP;

	default: throw;

	}
}



FVector EDirectionGetVector(EDirection Direction)
{
	return FVector(EDirectionGetVector2D(Direction), 0);
}

FVector2D EDirectionGetVector2D(EDirection Direction)
{
	switch (Direction)
	{
	case EDirection::XP: return FVector2D(1, 0);

	case EDirection::YP: return FVector2D(0, 1);

	case EDirection::XM: return FVector2D(-1, 0);

	case EDirection::YM: return FVector2D(0, -1);

	default: throw;
	}
}

EDirection EDirectionFromVector(FVector Vector)
{
	if (Vector == FVector(1, 0, 0)) return EDirection::XP;

	else if (Vector == FVector(-1, 0, 0)) return EDirection::XM;

	else if (Vector == FVector(0, 1, 0)) return EDirection::YP;

	else if (Vector == FVector(0, -1, 0)) return EDirection::YM;

	else throw;
}

EDirection EDirectionInvert(EDirection Direction)
{
	switch (Direction)
	{
	case EDirection::XP: return EDirection::XM;

	case EDirection::YP: return EDirection::YM;

	case EDirection::XM: return EDirection::XP;

	case EDirection::YM: return EDirection::YP;

	default: throw;
	}
	
}



//.........................................
//ComparateProperty
//.........................................

template<typename T>
bool ComparateProperty(UScriptStruct* Struct, const T& left, const T& right)
{
	for (TFieldIterator<UProperty> PropIt(Struct); PropIt; ++PropIt)
	{
		if (UIntProperty* IntProperty = Cast<UIntProperty>(*PropIt))
		{
			int LeftVal = IntProperty->GetPropertyValue_InContainer(&left);
			int RightVal = IntProperty->GetPropertyValue_InContainer(&right);
			if (LeftVal != RightVal) return false;
		}
		else if (UObjectProperty* ObjectPointerProperty = Cast<UObjectProperty>(*PropIt))
		{
			UObject* LeftVal = ObjectPointerProperty->GetPropertyValue_InContainer(&left);
			UObject* RightVal = ObjectPointerProperty->GetPropertyValue_InContainer(&right);
			if (LeftVal != RightVal) return false;
		}
		else if (UBoolProperty* BoolProperty = Cast<UBoolProperty>(*PropIt))
		{
			bool LeftVal = BoolProperty->GetPropertyValue_InContainer(&left);
			bool RightVal = BoolProperty->GetPropertyValue_InContainer(&right);
			if (LeftVal != RightVal) return false;
		}
		else if (UFloatProperty* FloatProp = Cast<UFloatProperty>(*PropIt))
		{
			float LeftVal = FloatProp->GetPropertyValue_InContainer(&left);
			float RightVal = FloatProp->GetPropertyValue_InContainer(&right);
			if (LeftVal != RightVal) return false;
		}
		else if (UNameProperty* NameProp = Cast<UNameProperty>(*PropIt))
		{
			FName LeftVal = NameProp->GetPropertyValue_InContainer(&left);
			FName RightVal = NameProp->GetPropertyValue_InContainer(&right);

			if (LeftVal != RightVal) return false;
		}
		else if (UStrProperty* StringProp = Cast<UStrProperty>(*PropIt))
		{
			FString LeftVal = StringProp->GetPropertyValue_InContainer(&left);
			FString RightVal = StringProp->GetPropertyValue_InContainer(&right);

			if (LeftVal != RightVal) return false;
		}
		else if (UArrayProperty* ArrayProp = Cast<UArrayProperty>(*PropIt))
		{

			const uint8* PropertyValueLeft = ArrayProp->ContainerPtrToValuePtr<uint8>(&left, 0);
			FScriptArrayHelper ArrayValueHelperLeft(ArrayProp, PropertyValueLeft);

			const uint8* PropertyValueRight = ArrayProp->ContainerPtrToValuePtr<uint8>(&right, 0);
			FScriptArrayHelper ArrayValueHelperRight(ArrayProp, PropertyValueRight);

			if (ArrayValueHelperRight.Num() != ArrayValueHelperLeft.Num())
			{
				return false;
			}

			for (int32 Idx = 0; Idx < ArrayValueHelperLeft.Num(); Idx++)
			{
				if (*ArrayValueHelperRight.GetRawPtr(Idx) != *ArrayValueHelperLeft.GetRawPtr(Idx))
				{
					return false;
				}
			}

		}
		else if (UEnumProperty* EnumProp = Cast<UEnumProperty>(*PropIt))
		{
			UNumericProperty* NumProp = EnumProp->GetUnderlyingProperty();
			UByteProperty* ByteProp = CastChecked<UByteProperty>(NumProp);

			uint8 LeftVal = ByteProp->GetPropertyValue_InContainer(&left);
			uint8 RightVal = ByteProp->GetPropertyValue_InContainer(&right);

			if (LeftVal != RightVal) return false;
		}
	}
	return true;
}

//.........................................
//TLevelMapBase
//.........................................

template <typename T>
void TLevelMapBase<T>::DestroyMap()
{
	for (int i = 0; i < MapCellData.size(); i++)
	{
		MapCellData[i].clear();
	}
}

//.........................................
//TLevelMap1D
//.........................................


template <typename T>
const std::unique_ptr<T>& TLevelMap1D<T>::GetCell(int X) const
{
	int Internal_X;
	ESemiAxis SemiAxis;

	std::tie(Internal_X, SemiAxis) = GetInternalCellAddres(X);

	if (Internal_X >= MapCellData[(int)SemiAxis].size())
	{
		throw;
	}
	
	return MapCellData[(int)SemiAxis][Internal_X];
}

template <typename T>
std::unique_ptr<T>& TLevelMap1D<T>::GetCell(int X)
{
	return  const_cast<std::unique_ptr<T>&>(static_cast<const TLevelMap1D<T>&>(*this).GetCell(X));
}

template <typename T>
bool TLevelMap1D<T>::IsCellExist(int X) const
{
	int Internal_X;
	ESemiAxis SemiAxis;

	std::tie(Internal_X, SemiAxis) = GetInternalCellAddres(X);

	if (Internal_X >= MapCellData[(int)SemiAxis].size())
	{
		return false;
	}

	return (bool)MapCellData[(int)SemiAxis][Internal_X];
}

template <typename T>
void TLevelMap1D<T>::FillRange(int Start, int End, const std::function<std::unique_ptr<T> (int X)> Filler)
{
	int Internal_X;
	ESemiAxis SemiAxis;

	std::tie(Internal_X, SemiAxis) = GetInternalCellAddres(Start);
	CheckArraySize(Internal_X, SemiAxis);

	std::tie(Internal_X, SemiAxis) = GetInternalCellAddres(End);
	CheckArraySize(Internal_X, SemiAxis);

	for (int i = Start; i <= End; i++)
	{
		

		std::tie(Internal_X,SemiAxis) = GetInternalCellAddres(i);

		if (!MapCellData[(int)SemiAxis][Internal_X])
		{
			MapCellData[(int)SemiAxis][Internal_X] = Filler(i);
		}
		
	}


}

template <typename T>
std::tuple<int, ESemiAxis> TLevelMap1D<T>::GetInternalCellAddres(int X) const
{
	if (X >= 0)
	{
		return std::make_tuple(X, ESemiAxis::Plus);
	}

	else 
	{
		return std::make_tuple(abs(X) - 1, ESemiAxis::Minus);
	}

}

template <typename T>
void TLevelMap1D<T>::CheckArraySize(int X, ESemiAxis SemiAxis)
{
	if (X >= MapCellData[(int)SemiAxis].size())
	{
		MapCellData[(int)SemiAxis].resize(X + 1);
	}
	
}

//.........................................
//TLevelMap2D
//.........................................


template <typename T>
const std::unique_ptr<T>& TLevelMap2D<T>::GetCell(int X, int Y) const
{
	int Internal_X, Internal_Y;
	EQuarterPlane QuarterPlane;

	std::tie(Internal_X, Internal_Y, QuarterPlane) = GetInternalCellAddres(X, Y);

	if (Internal_X >= MapCellData[(int)QuarterPlane].size())
	{
		throw;
	}
	else if (Internal_Y >= MapCellData[(int)QuarterPlane][Internal_X].size())
	{
		throw;
	}


	return MapCellData[(int)QuarterPlane][Internal_X][Internal_Y];
}

template <typename T>
std::unique_ptr<T>& TLevelMap2D<T>::GetCell(int X, int Y)
{
	int Internal_X, Internal_Y;
	EQuarterPlane QuarterPlane;

	std::tie(Internal_X, Internal_Y, QuarterPlane) = GetInternalCellAddres(X, Y);

	if (Internal_X >= MapCellData[(int)QuarterPlane].size())
	{
		CheckArraySize(Internal_X, Internal_Y, QuarterPlane);
	}
	else if (Internal_Y >= MapCellData[(int)QuarterPlane][Internal_X].size())
	{
		CheckArraySize(Internal_X, Internal_Y, QuarterPlane);
	}


	return MapCellData[(int)QuarterPlane][Internal_X][Internal_Y];
}

template <typename T>
bool TLevelMap2D<T>::IsCellExist(int X, int Y) const
{
	int Internal_X, Internal_Y;
	EQuarterPlane QuarterPlane;

	std::tie(Internal_X, Internal_Y, QuarterPlane) = GetInternalCellAddres(X, Y);

	if (Internal_X >= MapCellData[(int)QuarterPlane].size())
	{
		return false;
	}
	if (Internal_Y >= MapCellData[(int)QuarterPlane][Internal_X].size())
	{
		return false;
	}
	
	return (bool)MapCellData[(int)QuarterPlane][Internal_X][Internal_Y];
}

template <typename T>
void TLevelMap2D<T>::FillRange(FVector2D Start, FVector2D End, const std::function<std::unique_ptr<T>(int X, int Y)> Filler)
{
	int Internal_X, Internal_Y;
	EQuarterPlane QuarterPlane;

	
	ReservRange(Start, End);

	for (int i = Start.X; i <= End.X; i++)
	{
		for (int j = Start.Y; j <= End.Y; j++)
		{
			std::tie(Internal_X, Internal_Y, QuarterPlane) = GetInternalCellAddres(i, j);
			if (!MapCellData[(int)QuarterPlane][Internal_X][Internal_Y])
			{
				MapCellData[(int)QuarterPlane][Internal_X][Internal_Y] = Filler(i, j);
			}
		}
	}
	
	
}
template <typename T>
void TLevelMap2D<T>::ReservRange(FVector2D Start, FVector2D End)
{
	int Internal_X, Internal_Y;
	EQuarterPlane QuarterPlane;

	for (int i = Start.X; i <= End.X; i++)
	{
		std::tie(Internal_X, Internal_Y, QuarterPlane) = GetInternalCellAddres(i, Start.Y);
		CheckArraySize(Internal_X, Internal_Y, QuarterPlane);

		std::tie(Internal_X, Internal_Y, QuarterPlane) = GetInternalCellAddres(i, End.Y);
		CheckArraySize(Internal_X, Internal_Y, QuarterPlane);

	};
}

template <typename T>
std::tuple<int, int, EQuarterPlane> TLevelMap2D<T>::GetInternalCellAddres(int X, int Y) const
{
	if ((X >= 0) && (Y >= 0))
	{
		return std::make_tuple(X, Y, EQuarterPlane::PlusPlus);
	}

	else if ((X < 0) && (Y >= 0))
	{
		return std::make_tuple(abs(X) - 1, Y, EQuarterPlane::MinusPlus);
	}

	else if ((X >= 0) && (Y < 0))
	{
		return std::make_tuple(X, abs(Y) - 1, EQuarterPlane::PlusMinus);
	}

	return std::make_tuple(abs(X) - 1, abs(Y) - 1, EQuarterPlane::MinusMinus);

}

template <typename T>
void TLevelMap2D<T>::CheckArraySize(int X, int Y, EQuarterPlane QuarterPlane)
{
	if (X >= MapCellData[(int)QuarterPlane].size())
	{
		MapCellData[(int)QuarterPlane].resize(X + 1);

	}
	if (Y >= MapCellData[(int)QuarterPlane][X].size())
	{
		MapCellData[(int)QuarterPlane][X].resize(Y + 1);
	}
}


//***********************************************
//TDataStorageFromTexture
//***********************************************
template <typename T>
TDataStorageFromTexture<T>::TDataStorageFromTexture(FTexture2D* Texture, const std::function<T(FColor Color)> ColrToTypeConverter, std::mt19937& _RandomGenerator) :
	RandomGenerator(_RandomGenerator)
{
	std::vector<std::vector<FColor>> MapDataColor;

	GetDataFromTexture(MapDataColor, Texture);

	Data.resize(MapDataColor.size());

	for (int x = 0; x < MapDataColor.size(); x++)
	{
		for (int y = 0; y < MapDataColor[x].size(); y++)
		{

			Data[x].push_back(ColrToTypeConverter(MapDataColor[x][y]));
		}
	}

	MapOffset = GetRandomStartOffsetForMap();

}

template <typename T>
T TDataStorageFromTexture<T>::GetDataFromCoordinate(int X, int Y)
{
	std::pair<int, int> NewCoordinate = RecountCoordinateForMapSize(X + MapOffset.first, Y + MapOffset.second);

	return Data[NewCoordinate.first][NewCoordinate.second];
}

template <typename T>
void TDataStorageFromTexture<T>::GetDataFromTexture(std::vector<std::vector<FColor>>& TextureData, FTexture2D* Texture)
{
	TextureData.clear();


	if (!Texture)
	{
		throw;

	}
	else
	{
		int TextureSize = Texture->GetSizeX(); //the width of the texture

		std::vector<FColor> VTemp;

		FTexture2DMipMap& Mip = Texture->PlatformData->Mips[0];//A reference 
		void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
		uint8* raw = NULL;
		raw = (uint8*)Data;

		FColor pixel = FColor(0, 0, 0, 255);//used for spliting the data stored in raw form

		for (int x = 0; x < TextureSize; x++)
		{
			TextureData.push_back(VTemp);

			for (int y = 0; y < TextureSize; y++)
			{

				pixel.B = raw[4 * (TextureSize * y + x) + 0];
				pixel.G = raw[4 * (TextureSize * y + x) + 1];
				pixel.R = raw[4 * (TextureSize * y + x) + 2];

				TextureData[x].push_back(pixel);
			}
		}
		Mip.BulkData.Unlock();
		Texture->UpdateResource();
	}
}

template <typename T>
std::pair<int, int> TDataStorageFromTexture<T>::GetRandomStartOffsetForMap()
{
	std::uniform_int_distribution<> distrX(0, Data.size() - 1);
	std::uniform_int_distribution<> distrY(0, Data[0].size() - 1);

	int StartOffsetX = distrX(RandomGenerator);
	int StartOffsetY = distrY(RandomGenerator);

	return std::make_pair(StartOffsetX, StartOffsetY);
}

template <typename T>
std::pair<int, int> TDataStorageFromTexture<T>::RecountCoordinateForMapSize(int X, int Y)
{
	int NewX, NewY;
	int MapSizeX = Data.size();
	int MapSizeY = Data[0].size();

	if (X >= 0)
	{
		NewX = X % MapSizeX;
	}
	else
	{
		NewX = MapSizeX - 1 - (abs(X) - 1) % MapSizeX;
	}
	if (Y >= 0)
	{
		NewY = Y % MapSizeY;
	}
	else
	{
		NewY = MapSizeY - 1 - (abs(Y) - 1) % MapSizeY;
	}
	return std::make_pair(NewX, NewY);
}



//.........................................
//UObjectsDisposer
//.........................................

bool UObjectsDisposer::GetLocation(FVector2D& Location, FVector2D SearchStartCoordinate, int FloorForSearch, FVector2D SearchObjectSize,
	std::function<bool(FVector)>IsLocationFitPredicate) const
{
	FVector2D SearchAreaSize = GetSearchAreaSize();
	if (SearchObjectSize.X > SearchAreaSize.X) return false;
	if (SearchObjectSize.Y > SearchAreaSize.Y) return false;
	
	

	std::vector<FVector2D> CoordinatsForSearch;

	for (int r = 0; r < 7; r++)
	{
		CoordinatsForSearch.clear();

		GetRectangleCoordinate(CoordinatsForSearch, SearchStartCoordinate, r);

		for (int i = 0; i < CoordinatsForSearch.size(); i++)
		{
			if (CoordinatsForSearch[i].X >= SearchAreaRange.first.X  && CoordinatsForSearch[i].X + SearchObjectSize.X - 1 <= SearchAreaRange.second.X &&
				CoordinatsForSearch[i].Y >= SearchAreaRange.first.Y  && CoordinatsForSearch[i].Y + SearchObjectSize.Y - 1 <= SearchAreaRange.second.Y)
			{
				if (IsLocationFitPredicate(FVector(CoordinatsForSearch[i].X, CoordinatsForSearch[i].Y, FloorForSearch)))
				{
					Location = FVector2D(CoordinatsForSearch[i].X, CoordinatsForSearch[i].Y);
					return true;
				}
			}
			

		}

	}

	return false;
}

bool UObjectsDisposer::GetLocation(FVector& Location, FVector2D SearchStartCoordinate, FVector2D SearchObjectSize, std::function<bool(FVector)>IsLocationFitPredicate) const
{
	FVector2D SearchAreaSize = GetSearchAreaSize();
	if (SearchObjectSize.X > SearchAreaSize.X) return false;
	if (SearchObjectSize.Y > SearchAreaSize.Y) return false;

	std::vector<FVector2D> CoordinatsForSearch;

	for (int r = 0; r < 7; r++)
	{
		CoordinatsForSearch.clear();

		GetRectangleCoordinate(CoordinatsForSearch, SearchStartCoordinate, r);

		for (int i = 0; i < CoordinatsForSearch.size(); i++)
		{
			if (CoordinatsForSearch[i].X >= SearchAreaRange.first.X  && CoordinatsForSearch[i].X + SearchObjectSize.X - 1 <= SearchAreaRange.second.X &&
				CoordinatsForSearch[i].Y >= SearchAreaRange.first.Y  && CoordinatsForSearch[i].Y + SearchObjectSize.Y - 1 <= SearchAreaRange.second.Y)
			{
				for (int j = LevelSettings.FloorNum - 1; j > -1; j--)
				{
					if (IsLocationFitPredicate(FVector(CoordinatsForSearch[i].X, CoordinatsForSearch[i].Y, j)))
					{
						Location = FVector(CoordinatsForSearch[i].X, CoordinatsForSearch[i].Y, j);
						return true;
					}
				}
				
			}


		}

	}

	return false;
}


void UObjectsDisposer::GetRectangleCoordinate(std::vector<FVector2D>& Coordinats, FVector2D Center, int Radius) const
{
	Coordinats.clear();
	if (Radius == 0)
	{
		Coordinats.push_back(Center);
		return;
	}

	switch (GetRandomNumber(0, 3))
	{
	case 0:

		GetXPlus(Coordinats, Center, Radius);
		GetYPlus(Coordinats, Center, Radius);
		GetXMinus(Coordinats, Center, Radius);
		GetYMinus(Coordinats, Center, Radius);
		break;

	case 1:

		GetYMinus(Coordinats, Center, Radius);
		GetXPlus(Coordinats, Center, Radius);
		GetYPlus(Coordinats, Center, Radius);
		GetXMinus(Coordinats, Center, Radius);
		break;

	case 2:

		GetXMinus(Coordinats, Center, Radius);
		GetYMinus(Coordinats, Center, Radius);
		GetXPlus(Coordinats, Center, Radius);
		GetYPlus(Coordinats, Center, Radius);
		break;

	case 3:

		GetYPlus(Coordinats, Center, Radius);
		GetXMinus(Coordinats, Center, Radius);
		GetYMinus(Coordinats, Center, Radius);
		GetXPlus(Coordinats, Center, Radius);
		break;
	}

}

void UObjectsDisposer::GetXPlus(std::vector<FVector2D>& Coordinats, FVector2D Center, int Radius) const
{
	for (int i = -Radius; i < Radius; i++)
	{
		Coordinats.push_back(FVector2D(Center.X + Radius, Center.Y + i));
	}
}

void UObjectsDisposer::GetYPlus(std::vector<FVector2D>& Coordinats, FVector2D Center, int Radius) const
{
	for (int i = Radius; i > -Radius; i--)
	{
		Coordinats.push_back(FVector2D(Center.X + i, Center.Y + Radius));
	}
}

void UObjectsDisposer::GetXMinus(std::vector<FVector2D>& Coordinats, FVector2D Center, int Radius) const
{
	for (int i = Radius; i > -Radius; i--)
	{
		Coordinats.push_back(FVector2D(Center.X - Radius, Center.Y + i));
	}
}

void UObjectsDisposer::GetYMinus(std::vector<FVector2D>& Coordinats, FVector2D Center, int Radius) const
{
	for (int i = -Radius; i < Radius; i++)
	{
		Coordinats.push_back(FVector2D(Center.X + i, Center.Y - Radius));
	}
}

int UObjectsDisposer::GetRandomNumber(int From, int To) const
{
	std::uniform_int_distribution<> distr(From, To);
	return distr(RandomGenerator);
}



bool GetIntervalIntesect(std::pair<int, int>& IntersecInterval, std::pair<int, int> FirstInterval, std::pair<int, int>SecondInterval)
{

	if (FirstInterval.second < SecondInterval.first || FirstInterval.first > SecondInterval.second) return false;

	IntersecInterval.first = FirstInterval.first < SecondInterval.first ? SecondInterval.first : FirstInterval.first;

	IntersecInterval.second = FirstInterval.second < SecondInterval.second ? FirstInterval.second : SecondInterval.second;

	return true;
}

std::pair<int, int> GetCircleCoordinateFromX(int X, FVector2D CenterCoordinate, int Radius)
{

	float Rez = pow(Radius, 2) - pow(X - CenterCoordinate.X, 2);
	if (Rez < 0) throw;
	Rez = sqrt(Rez);
	float Y1 = Rez + CenterCoordinate.Y;
	float Y2 = -Rez + CenterCoordinate.Y;

	return std::make_pair(RoundNumber(Y1), RoundNumber(Y2));
}

bool  IsPointInCircle(int X, int Y, FVector2D CenterCoordinate, int Radius)
{

	if (Y > CenterCoordinate.Y + Radius || Y < CenterCoordinate.Y - Radius) return false;

	if (X > CenterCoordinate.X + Radius || X < CenterCoordinate.X - Radius) return false;

	std::pair<int, int> YCoord = GetCircleCoordinateFromX(X, CenterCoordinate, Radius);

	if (Y > YCoord.first || Y < YCoord.second) return false;

	return true;
}