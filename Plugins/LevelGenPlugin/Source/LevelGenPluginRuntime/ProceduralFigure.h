// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "CoreMinimal.h"
#include <set>
#include <memory>
#include "LevelGenCore.h"
struct FProcMeshTangent;
struct FProcMeshSection;
class FProceduralFigureBase;
struct FLevelGenProcMeshSection;

class FProceduralFigurBuffer
{
public:
	FProceduralFigurBuffer(FVector _StartCoordinate) : StartCoordinate(_StartCoordinate) {}

	void AddWallSegments(std::map<const FLevelGeneratorMaterialSettings*, std::shared_ptr<FProceduralFigureBase>>& WallSegments, FVector FigureCentrCoordinate);

	void AddProceduralFigure(FProceduralFigureBase& Figure, FVector FigureSentrCoordinate, const FLevelGeneratorMaterialSettings* Material);

	const std::map<const FLevelGeneratorMaterialSettings*, std::shared_ptr<FProceduralFigureBase>>& GetBuffer() const { return FigureBuffer; }

private:

	void IniciateBufferForCurrentMaterial(const FLevelGeneratorMaterialSettings* Material);
	
private:
	FVector StartCoordinate;

	std::map<const FLevelGeneratorMaterialSettings*, std::shared_ptr<FProceduralFigureBase>> FigureBuffer;
};


class FCoordinateSystem2D
{
public:
	FCoordinateSystem2D(FVector BeginCoordinate, FVector Normal, FVector Tangent);

	FCoordinateSystem2D(FVector FirstPoint, FVector2D FirstPoint2D, FVector Normal, FVector Tangent);

	FVector2D Get2DCoordinateFrom3D(FVector TransformPoint);

	FVector Get3DCoordinateFrom2D(FVector2D TransformPoint);

	bool IsPointOnPlane(FVector Point, bool Exactly = false);

	FVector GetNormals() { return N; }

	FVector GetTangent() { return i; }

private:
	FVector BeginCoordinate;
	FVector i;
	FVector N;
	FVector j;

};


struct FProceduralVertex
{
	FProceduralVertex(FVector _Coordinat, FVector _Normal, FVector _Tangent) : Coordinat(_Coordinat), Normal(_Normal), Tangent(_Tangent) {}

	FProceduralVertex(FVector _Coordinat, FVector2D _UVCoordinat, FVector _Normal, FVector _Tangent) : Coordinat(_Coordinat), UVCoordinate(true, _UVCoordinat), Normal(_Normal), Tangent(_Tangent) {}

	FVector Coordinat;

	mutable std::pair<bool, FVector2D> NotOwnUVCoordinate = std::make_pair(false, FVector2D(0, 0));

	mutable std::pair<bool, FVector2D> UVCoordinate = std::make_pair(false, FVector2D(0, 0));

	mutable FVector Normal;

	mutable FVector Tangent;

	mutable FCoordinateSystem2D* UVCoordinateSystem = nullptr;

	bool IsCoordinateSystemFit(FCoordinateSystem2D* CoordinateSystem) const { return CoordinateSystem->IsPointOnPlane(Coordinat) && CoordinateSystem->GetNormals() == Normal; }

	const bool operator<(const FProceduralVertex& rhs) const
	{
		if (Coordinat.X != rhs.Coordinat.X)
		{
			return Coordinat.X < rhs.Coordinat.X;
		}
		else if (Coordinat.Y != rhs.Coordinat.Y)
		{
			return Coordinat.Y < rhs.Coordinat.Y;
		}
		else if (Coordinat.Z != rhs.Coordinat.Z)
		{
			return Coordinat.Z < rhs.Coordinat.Z;
		}
		else if (Normal.X != rhs.Normal.X)
		{
			return Normal.X < rhs.Normal.X;
		}
		else if (Normal.Y != rhs.Normal.Y)
		{
			return Normal.Y < rhs.Normal.Y;
		}
		else if (Normal.Z != rhs.Normal.Z)
		{
			return Normal.Z < rhs.Normal.Z;
		}
		return false;
	}
};



class FProceduralFigureBase
{


public:
	FProceduralFigureBase(UMaterialInterface* _Material, float _TextureCodfficent) :  Material(_Material), TextureCodfficent(_TextureCodfficent) {}

	virtual ~FProceduralFigureBase() {}

	void Merge(FProceduralFigureBase& Figure, FVector RelatevCoordinate = FVector(0, 0, 0));


	void GetMeshSection(FProcMeshSection& Section) const;

	void GetLevelGenMeshSection(FLevelGenProcMeshSection& Section) const;

	void GetVertices(TArray<FVector>& Vertices) const;

	void GetNormals(TArray<FVector>& Normals) const;

	void GetTangents(TArray<FVector>& Tangents) const;

	void GetTriangles(TArray<uint32>& Triangles) const;

	void GetUVCoordinate(TArray<FVector2D>& UVCoordinate) const;

	const UMaterialInterface* GetMaterial() const { return Material; }

	FBox GetLocalBox() const;

	void SetCollision(bool EnabelCollision) { Collision = EnabelCollision; }

	bool GetCollision() const { return Collision; }

	int GetTriangleNumber() const { return Triangles.size() / 3; }

	int GetVertexNumber() const { return Vertices.size(); }

	const std::set<FProceduralVertex>& GetVertices() const { return Vertices; }

	float GetTextureCodfficent() const { return TextureCodfficent; }

	virtual std::unique_ptr<FProceduralFigureBase> Clone() const  { return std::unique_ptr<FProceduralFigureBase>(new FProceduralFigureBase(*this)); }

protected:

	void AddCoordinateSystem(std::shared_ptr<FCoordinateSystem2D> _Coord2D) { Coord2D.push_back(_Coord2D); }

	void AddVertex(FVector Coordinate, FVector2D UVCoordinate, FVector Normal, FVector Tangent);

	void AddTriangle(int Vertex_1, int Vertex_2, int Vertex_3);

	void GetNewTrianglesWithOutUVCoordinateSystem(const TArray<uint32>& TrianglesBuf, std::multimap<int, std::vector<uint32>>& NewTriangles);

	void CreateUVCoordinateForNewTriangles(TArray<uint32>& TrianglesBuf);

	FCoordinateSystem2D* GetCoord2DSysForPointAndNormal(FVector Point, FVector Normal);

	FCoordinateSystem2D* GetCoord2DSysForPoint(FVector Point);

	void RefreshTrianglesWithNewCoordinateSystem(TArray<uint32>& TrianglesBuf, FCoordinateSystem2D* CoordinateSystem);



private:

	UMaterialInterface * Material = nullptr;

	std::vector<int32> Triangles;

	float TextureCodfficent = 1;

	std::set<FProceduralVertex> Vertices;

	std::vector<const FProceduralVertex*> VertexOrder;

	std::vector<std::shared_ptr<FCoordinateSystem2D>> Coord2D;

	bool Collision = false;
};


class FProceduralFigureTriangle : public FProceduralFigureBase
{
public:
	FProceduralFigureTriangle(FVector Coordinate_1, FVector Coordinate_2, FVector Coordinate_3, FLevelGeneratorMaterialSettings Material, FVector Normal, FVector Tangent);

	virtual std::unique_ptr<FProceduralFigureBase> Clone() const override { return std::unique_ptr<FProceduralFigureBase>(new FProceduralFigureTriangle(*this)); }

};

class FProceduralFigureRectangle : public FProceduralFigureBase
{
public:
	FProceduralFigureRectangle(FVector Coordinate_1, FVector Coordinate_2, FVector Coordinate_3, FVector Coordinate_4, FLevelGeneratorMaterialSettings Material, FVector Normal, FVector Tangent);

	FProceduralFigureRectangle(FVector Point, FVector Normal, FVector i, FVector2D Size, FLevelGeneratorMaterialSettings Material);

	virtual std::unique_ptr<FProceduralFigureBase> Clone() const override { return std::unique_ptr<FProceduralFigureBase>(new FProceduralFigureRectangle(*this)); }
};