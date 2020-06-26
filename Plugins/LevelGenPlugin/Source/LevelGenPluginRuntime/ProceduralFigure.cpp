// Copyright 2018 Pavlov Dmitriy
#include "ProceduralFigure.h"
#include "ProceduralMeshComponent.h"
#include "LevelGenProceduralMeshComponent.h"
#include "LevelGeneratorSettings.h"

//.........................................
//FProceduralFigurBuffer
//.........................................

void FProceduralFigurBuffer::AddWallSegments(std::map<const FLevelGeneratorMaterialSettings*, std::shared_ptr<FProceduralFigureBase>>& WallSegments, FVector FigureCentrCoordinate)
{
	for (auto CurrentSegment : WallSegments)
	{
		AddProceduralFigure(*CurrentSegment.second, FigureCentrCoordinate, CurrentSegment.first);
	}
}


void FProceduralFigurBuffer::AddProceduralFigure(FProceduralFigureBase& Figure, FVector FigureCentrCoordinate, const FLevelGeneratorMaterialSettings* Material)
{
	
	if (FigureBuffer.find(Material) == FigureBuffer.end())
	{
		IniciateBufferForCurrentMaterial(Material);
	}

	FVector DeltaCoordinate = FigureCentrCoordinate - StartCoordinate;

	FigureBuffer[Material]->Merge(Figure, DeltaCoordinate);
	
}

void FProceduralFigurBuffer::IniciateBufferForCurrentMaterial(const FLevelGeneratorMaterialSettings* Material)
{
	std::shared_ptr<FProceduralFigureBase> NewFigure(new FProceduralFigureBase(Material->Material, Material->MaterialCoefficient));

	FigureBuffer.insert(std::make_pair(Material, NewFigure));
}

//.........................................
//FCoordinateSystem2D
//.........................................

FCoordinateSystem2D::FCoordinateSystem2D(FVector _BeginCoordinate, FVector Normal, FVector Tangent) :
	BeginCoordinate(_BeginCoordinate),  i(Tangent), N(Normal)
{

	j = FVector::CrossProduct(i, N);
	j = j / j.Size();
}

FCoordinateSystem2D::FCoordinateSystem2D(FVector FirstPoint, FVector2D FirstPoint2D, FVector Normal, FVector Tangent) :  i(Tangent), N(Normal)
{

	j = FVector::CrossProduct(i, N);
	j = j / j.Size();

	BeginCoordinate = FirstPoint - FirstPoint2D.X * i - FirstPoint2D.Y * j;
}

FVector2D FCoordinateSystem2D::Get2DCoordinateFrom3D(FVector TransformPoint)
{
	FVector TransformVector = TransformPoint - BeginCoordinate;
	return  FVector2D(FVector::DotProduct(TransformVector, i), FVector::DotProduct(TransformVector, j));
}

FVector FCoordinateSystem2D::Get3DCoordinateFrom2D(FVector2D TransformPoint)
{
	return BeginCoordinate + (TransformPoint.X) * i + (TransformPoint.Y) * j;
}

bool FCoordinateSystem2D::IsPointOnPlane(FVector Point, bool Exactly)
{
	FVector PointVector = Point - BeginCoordinate;

	if (Exactly)
	{
		return FVector::DotProduct(PointVector, N) == 0;
	}
	else
	{
		return abs(FVector::DotProduct(PointVector, N)) < 20;
	}

}




//.........................................
//FProceduralFigureBase
//.........................................


void FProceduralFigureBase::Merge(FProceduralFigureBase& Figure, FVector RelatevCoordinate)
{
	TArray<FVector> VerticesBuf;
	TArray<FVector> NormalBuf;
	TArray<FVector> TangentBuf;
	TArray<uint32> TrianglesBuf;

	Figure.GetVertices(VerticesBuf);
	Figure.GetNormals(NormalBuf);
	Figure.GetTangents(TangentBuf);
	Figure.GetTriangles(TrianglesBuf);

	
	int NewTrianglesOffset = VertexOrder.size();

	for (int i = 0; i < TrianglesBuf.Num(); i++)
	{
		TrianglesBuf[i] = TrianglesBuf[i] + NewTrianglesOffset;
		Triangles.push_back(TrianglesBuf[i]);
	}



	for (int i = 0; i < VerticesBuf.Num(); i++)
	{

		if (FCoordinateSystem2D* Coord2DLoc = GetCoord2DSysForPointAndNormal(VerticesBuf[i] + RelatevCoordinate, NormalBuf[i]))
		{
			FProceduralVertex NewVert(VerticesBuf[i] + RelatevCoordinate, NormalBuf[i], TangentBuf[i]);

			NewVert.UVCoordinateSystem = Coord2DLoc;

			NewVert.UVCoordinate = std::make_pair(true, Coord2DLoc->Get2DCoordinateFrom3D(VerticesBuf[i] + RelatevCoordinate));

			VertexOrder.push_back(&*Vertices.insert(NewVert).first);
		}
		else if (FCoordinateSystem2D* Coord2DLoc2 = GetCoord2DSysForPoint(VerticesBuf[i] + RelatevCoordinate))
		{
			FProceduralVertex NewVert(VerticesBuf[i] + RelatevCoordinate, NormalBuf[i], TangentBuf[i]);

			NewVert.NotOwnUVCoordinate = std::make_pair(true, Coord2DLoc2->Get2DCoordinateFrom3D(VerticesBuf[i] + RelatevCoordinate));

			VertexOrder.push_back(&*Vertices.insert(NewVert).first);
		}
		else
		{
			FProceduralVertex NewVert(VerticesBuf[i] + RelatevCoordinate, NormalBuf[i], TangentBuf[i]);

			VertexOrder.push_back(&*Vertices.insert(NewVert).first);

		}

	}

	CreateUVCoordinateForNewTriangles(TrianglesBuf);
}




void FProceduralFigureBase::CreateUVCoordinateForNewTriangles(TArray<uint32>& TrianglesBuf)
{

	for (int i = 0; i < 10; i++)
	{
		std::multimap<int, std::vector<uint32>> NewTriangles;

		GetNewTrianglesWithOutUVCoordinateSystem(TrianglesBuf, NewTriangles);

		if (NewTriangles.count(3) != 0)
		{
			std::pair<std::multimap<int, std::vector<uint32>>::iterator, std::multimap<int, std::vector<uint32>>::iterator> it = NewTriangles.equal_range(3);
			const FProceduralVertex* BasePoint = VertexOrder[it.first->second[0]];

			std::shared_ptr<FCoordinateSystem2D> Coord2DNew(new FCoordinateSystem2D(BasePoint->Coordinat, BasePoint->NotOwnUVCoordinate.second, BasePoint->Normal, BasePoint->Tangent));

			AddCoordinateSystem(Coord2DNew);

			RefreshTrianglesWithNewCoordinateSystem(TrianglesBuf, Coord2DNew.get());

		}
		else if (NewTriangles.count(2) != 0)
		{
			std::pair<std::multimap<int, std::vector<uint32>>::iterator, std::multimap<int, std::vector<uint32>>::iterator> it = NewTriangles.equal_range(2);
			const FProceduralVertex* BasePoint = nullptr;


			for (int j = 0; j < it.first->second.size(); j++)
			{
				if (VertexOrder[it.first->second[j]]->NotOwnUVCoordinate.first)
				{

					BasePoint = VertexOrder[it.first->second[j]];
					break;
				}

			}

			std::shared_ptr<FCoordinateSystem2D> Coord2DNew(new FCoordinateSystem2D(BasePoint->Coordinat, BasePoint->UVCoordinate.second, BasePoint->Normal, BasePoint->Tangent));

			AddCoordinateSystem(Coord2DNew);

			RefreshTrianglesWithNewCoordinateSystem(TrianglesBuf, Coord2DNew.get());
		}
		else if (NewTriangles.count(1) != 0)
		{
			std::pair<std::multimap<int, std::vector<uint32>>::iterator, std::multimap<int, std::vector<uint32>>::iterator> it = NewTriangles.equal_range(1);
			const FProceduralVertex* BasePoint = nullptr;


			for (int j = 0; j < it.first->second.size(); j++)
			{
				if (VertexOrder[it.first->second[j]]->NotOwnUVCoordinate.first)
				{

					BasePoint = VertexOrder[it.first->second[j]];
					break;
				}

			}

			std::shared_ptr<FCoordinateSystem2D> Coord2DNew(new FCoordinateSystem2D(BasePoint->Coordinat, BasePoint->UVCoordinate.second, BasePoint->Normal, BasePoint->Tangent));

			AddCoordinateSystem(Coord2DNew);

			RefreshTrianglesWithNewCoordinateSystem(TrianglesBuf, Coord2DNew.get());
		}
		else if (NewTriangles.count(0) != 0)
		{
			std::pair<std::multimap<int, std::vector<uint32>>::iterator, std::multimap<int, std::vector<uint32>>::iterator> it = NewTriangles.equal_range(0);

			std::shared_ptr<FCoordinateSystem2D> Coord2DNew(new FCoordinateSystem2D(VertexOrder[it.first->second[0]]->Coordinat, VertexOrder[it.first->second[0]]->Normal, VertexOrder[it.first->second[0]]->Tangent));

			AddCoordinateSystem(Coord2DNew);

			RefreshTrianglesWithNewCoordinateSystem(TrianglesBuf, Coord2DNew.get());
		}
		else
		{
			return; // All triangles check
		}


	}

	UE_LOG(LogTemp, Warning, TEXT("Too many cycles in merge function")); // Cann't check coordinate

}

void FProceduralFigureBase::GetNewTrianglesWithOutUVCoordinateSystem(const TArray<uint32>& TrianglesBuf, std::multimap<int, std::vector<uint32>>& NewTriangles)
{
	for (int i = 0; i < TrianglesBuf.Num() / 3; i++)  // Get num triangles in new figure
	{
		if (!VertexOrder[TrianglesBuf[i * 3]]->UVCoordinateSystem)
		{
			int VertexWithCoordinat = 0;
			for (int j = 0; j < 3; j++)
			{
				if (VertexOrder[TrianglesBuf[i * 3 + j]]->NotOwnUVCoordinate.first)
				{

					VertexWithCoordinat++;
				}
			}
			NewTriangles.insert(std::make_pair(VertexWithCoordinat, std::vector<uint32>{TrianglesBuf[i * 3], TrianglesBuf[i * 3 + 1], TrianglesBuf[i * 3 + 2]}));

		}

	}
}

FCoordinateSystem2D* FProceduralFigureBase::GetCoord2DSysForPointAndNormal(FVector Point, FVector Normal)
{

	for (int j = 0; j < Coord2D.size(); j++)
	{
		if (Coord2D[j]->IsPointOnPlane(Point))
		{
			if (Normal == Coord2D[j]->GetNormals())
			{
				return Coord2D[j].get();
			}

		}
	}

	return  nullptr;
}

FCoordinateSystem2D* FProceduralFigureBase::GetCoord2DSysForPoint(FVector Point)
{

	for (int j = 0; j < Coord2D.size(); j++)
	{
		if (Coord2D[j]->IsPointOnPlane(Point))
		{
			return Coord2D[j].get();

		}
	}

	return  nullptr;
}

void FProceduralFigureBase::RefreshTrianglesWithNewCoordinateSystem(TArray<uint32>& TrianglesBuf, FCoordinateSystem2D* CoordinateSystem)
{
	for (int i = 0; i < TrianglesBuf.Num() / 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (!VertexOrder[TrianglesBuf[i * 3 + j]]->UVCoordinateSystem && VertexOrder[TrianglesBuf[i * 3 + j]]->IsCoordinateSystemFit(CoordinateSystem))
			{
				VertexOrder[TrianglesBuf[i * 3 + j]]->UVCoordinateSystem = CoordinateSystem;

				VertexOrder[TrianglesBuf[i * 3 + j]]->UVCoordinate = std::make_pair(true, CoordinateSystem->Get2DCoordinateFrom3D(VertexOrder[TrianglesBuf[i * 3 + j]]->Coordinat));
			}
		}

	}

}
void FProceduralFigureBase::GetMeshSection(FProcMeshSection& Section) const
{
	Section.ProcVertexBuffer.Reserve(Vertices.size());


	for (auto it = Vertices.begin(); it != Vertices.end(); it++)
	{
		FProcMeshVertex NewVertex;

		NewVertex.Position = it->Coordinat;
		NewVertex.Normal = it->Normal;
		NewVertex.UV0 = it->UVCoordinate.second * TextureCodfficent;
		NewVertex.Tangent = FProcMeshTangent(it->Tangent, false);

		Section.ProcVertexBuffer.Add(NewVertex);

		Section.SectionLocalBox += NewVertex.Position;
	}

	Section.bEnableCollision = GetCollision();

	GetTriangles(Section.ProcIndexBuffer);

}

void FProceduralFigureBase::GetLevelGenMeshSection(FLevelGenProcMeshSection& Section) const
{
	Section.ProcVertexBuffer.Reserve(Vertices.size());
	

	for (auto it = Vertices.begin(); it != Vertices.end(); it++)
	{
		FLevelGenProcMeshVertex NewVertex;

		NewVertex.Position = it->Coordinat;
		NewVertex.Normal = it->Normal;
		NewVertex.UV0 = it->UVCoordinate.second * TextureCodfficent;
		NewVertex.Tangent = it->Tangent;

		Section.ProcVertexBuffer.Add(NewVertex);

		Section.SectionLocalBox += NewVertex.Position;
	}

	Section.bEnableCollision = GetCollision();

	GetTriangles(Section.ProcIndexBuffer);

}

FBox FProceduralFigureBase::GetLocalBox() const
{
	FBox NewBox;

	for (auto it = Vertices.begin(); it != Vertices.end(); it++)
	{
		NewBox += it->Coordinat;
		
	}
	return NewBox;
}

void FProceduralFigureBase::GetVertices(TArray<FVector>& _Vertices) const
{
	_Vertices.Reserve(Vertices.size());
	for (auto it = Vertices.begin(); it != Vertices.end(); it++)
	{
		_Vertices.Add(it->Coordinat);
	}

}

void FProceduralFigureBase::GetNormals(TArray<FVector>& Normals) const
{
	Normals.Reserve(Vertices.size());
	for (auto it = Vertices.begin(); it != Vertices.end(); it++)
	{
		Normals.Add(it->Normal);
	}

}

void FProceduralFigureBase::GetTangents(TArray<FVector>& Tangents) const
{
	Tangents.Reserve(Vertices.size());
	for (auto it = Vertices.begin(); it != Vertices.end(); it++)
	{
		Tangents.Add(it->Tangent);
	}
}

void FProceduralFigureBase::GetTriangles(TArray<uint32>& _Triangles) const
{
	_Triangles.Reserve(Triangles.size());

	std::multimap<const FProceduralVertex*, int> VertexOrderMap;
	std::map<int, int> RealVertixConverter;

	for (int i = 0; i < VertexOrder.size(); i++)
	{
		VertexOrderMap.insert(std::make_pair(VertexOrder[i], i));
	}

	int k = 0;
	std::pair<std::multimap<const FProceduralVertex*, int>::iterator, std::multimap<const FProceduralVertex*, int>::iterator> MultiIt;
	for (auto it = Vertices.begin(); it != Vertices.end(); it++)
	{
		MultiIt = VertexOrderMap.equal_range(&*it);
		for (auto Mit = MultiIt.first; Mit != MultiIt.second; Mit++)
		{

			RealVertixConverter.insert(std::make_pair(Mit->second, k));

		}
		k++;

	}

	for (int i = 0; i < Triangles.size(); i++)
	{
		_Triangles.Add(RealVertixConverter[Triangles[i]]);
	}

}

void FProceduralFigureBase::GetUVCoordinate(TArray<FVector2D>& UVCoordinate) const
{
	UVCoordinate.Reserve(Vertices.size());
	for (auto it = Vertices.begin(); it != Vertices.end(); it++)
	{
		UVCoordinate.Add(it->UVCoordinate.second * TextureCodfficent);
	}
}

void FProceduralFigureBase::AddVertex(FVector Coordinate, FVector2D UVCoordinate, FVector Normal, FVector Tangent)
{

	VertexOrder.push_back(&*Vertices.insert(FProceduralVertex(Coordinate, UVCoordinate, Normal, Tangent)).first);
}


void FProceduralFigureBase::AddTriangle(int Vertex_1, int Vertex_2, int Vertex_3)
{
	Triangles.push_back(Vertex_1);
	Triangles.push_back(Vertex_2);
	Triangles.push_back(Vertex_3);
}

//.........................................
//FProceduralFigureTriangle
//.........................................

FProceduralFigureTriangle::FProceduralFigureTriangle(FVector Coordinate1, FVector Coordinate2, FVector Coordinate3, FLevelGeneratorMaterialSettings Material, FVector Normal, FVector Tangent) :
	FProceduralFigureBase(Material.Material, Material.MaterialCoefficient)
{
	std::shared_ptr<FCoordinateSystem2D> Coord2D_loc(new FCoordinateSystem2D(Coordinate1, Normal, Tangent));
	std::shared_ptr<FCoordinateSystem2D> Coord2DPerpendicular;
	
	AddCoordinateSystem(Coord2D_loc);

	AddVertex(Coordinate1, FVector2D(0, 0), Normal, Tangent);
	AddVertex(Coordinate2, Coord2D_loc->Get2DCoordinateFrom3D(Coordinate2), Normal, Tangent);
	AddVertex(Coordinate3, Coord2D_loc->Get2DCoordinateFrom3D(Coordinate3), Normal, Tangent);


	FVector Vec1 = Coordinate2 - Coordinate1;
	FVector Vec2 = Coordinate3 - Coordinate1;
	FVector Vec3 = FVector::CrossProduct(Vec1, Vec2);
	Vec3 = Vec3 / Vec3.Size();

	if (Vec3 == Normal)
	{
		AddTriangle(0, 2, 1);
		
	}
	else
	{
		AddTriangle(0, 1, 2);
	}

}

FProceduralFigureRectangle::FProceduralFigureRectangle(FVector Coordinate_1, FVector Coordinate_2, FVector Coordinate_3, FVector Coordinate_4, FLevelGeneratorMaterialSettings Material,
	FVector Normal, FVector Tangent) :
	FProceduralFigureBase(Material.Material, Material.MaterialCoefficient)
{
	std::shared_ptr<FCoordinateSystem2D> Coord2D_loc(new FCoordinateSystem2D(Coordinate_1, Normal, Tangent));
	std::shared_ptr<FCoordinateSystem2D> Coord2DPerpendicular;
	
	AddCoordinateSystem(Coord2D_loc);
	AddVertex(Coordinate_1, FVector2D(0, 0), Normal, Tangent);
	AddVertex(Coordinate_2, Coord2D_loc->Get2DCoordinateFrom3D(Coordinate_2), Normal, Tangent);
	AddVertex(Coordinate_3, Coord2D_loc->Get2DCoordinateFrom3D(Coordinate_3), Normal, Tangent);


	if (Coord2D_loc->IsPointOnPlane(Coordinate_4))
	{
		AddVertex(Coordinate_4, Coord2D_loc->Get2DCoordinateFrom3D(Coordinate_4), Normal, Tangent);
	}
	else
	{
		std::shared_ptr<FCoordinateSystem2D> Coord2DNew(new FCoordinateSystem2D(Coordinate_2, Coord2D_loc->Get2DCoordinateFrom3D(Coordinate_2), Normal, Tangent));

		AddCoordinateSystem(Coord2DNew);
		AddVertex(Coordinate_4, Coord2DNew->Get2DCoordinateFrom3D(Coordinate_4), Normal, Tangent);

	}

	AddTriangle(0, 1, 2);
	AddTriangle(0, 2, 3);
}


FProceduralFigureRectangle::FProceduralFigureRectangle(FVector Point, FVector Normal, FVector i, FVector2D Size, FLevelGeneratorMaterialSettings Material) :
	FProceduralFigureBase(Material.Material, Material.MaterialCoefficient)

{
	std::shared_ptr<FCoordinateSystem2D> Coord2D_loc(new FCoordinateSystem2D(Point, Normal, i));

	FVector2D P1(-Size.X / 2, -Size.Y / 2);
	FVector2D P2(Size.X / 2, -Size.Y / 2);
	FVector2D P3(Size.X / 2, Size.Y / 2);
	FVector2D P4(-Size.X / 2, Size.Y / 2);

	FVector V1 = Coord2D_loc->Get3DCoordinateFrom2D(P1);
	FVector V2 = Coord2D_loc->Get3DCoordinateFrom2D(P2);
	FVector V3 = Coord2D_loc->Get3DCoordinateFrom2D(P3);
	FVector V4 = Coord2D_loc->Get3DCoordinateFrom2D(P4);

	
	AddVertex(V1, P1, Normal, i);
	AddVertex(V2, P2, Normal, i);
	AddVertex(V3, P3, Normal, i);
	AddVertex(V4, P4, Normal, i);

	AddCoordinateSystem(Coord2D_loc);
	

	AddTriangle(0, 1, 2);
	AddTriangle(0, 2, 3);
}
