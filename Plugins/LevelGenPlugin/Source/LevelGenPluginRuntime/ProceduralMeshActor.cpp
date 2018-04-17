// Copyright 2018 Pavlov Dmitriy
#include "ProceduralMeshActor.h"
#include "ProceduralFigure.h"
#include "LevelGeneratorSettings.h"

/**
*	Create/replace a section for this procedural mesh component.
*	@param	SectionIndex		Index of the section to create or replace.
*	@param	Vertices			Vertex buffer of all vertex positions to use for this mesh section.
*	@param	Triangles			Index buffer indicating which vertices make up each triangle. Length must be a multiple of 3.
*	@param	Normals				Optional array of normal vectors for each vertex. If supplied, must be same length as Vertices array.
*	@param	UV0					Optional array of texture co-ordinates for each vertex. If supplied, must be same length as Vertices array.
*	@param	VertexColors		Optional array of colors for each vertex. If supplied, must be same length as Vertices array.
*	@param	Tangents			Optional array of tangent vector for each vertex. If supplied, must be same length as Vertices array.
*	@param	bCreateCollision	Indicates whether collision should be created for this section. This adds significant cost.
*/



ALevelGenProceduralMeshActor::ALevelGenProceduralMeshActor()
{
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	RootComponent = ProceduralMesh;
	ProceduralMesh->bCastStaticShadow = false;
}

void ALevelGenProceduralMeshActor::AddMesh(const FProceduralFigurBuffer& WallBuffer)
{
	const std::map<const FLevelGeneratorMaterialSettings*, std::shared_ptr<FProceduralFigureBase>>& Meshes = WallBuffer.GetBuffer();

	for (auto CurrentMesh = Meshes.begin(); CurrentMesh != Meshes.end(); CurrentMesh++)
	{
		AddMesh(CurrentMesh->second);
		
	}

}

void ALevelGenProceduralMeshActor::AddMesh(std::shared_ptr<FProceduralFigureBase> Mesh)
{
	TArray<FVector> Vertices;
	TArray<FVector> Normals;
	TArray<FVector2D> UVCoordinate;
	TArray<int32> Triangles;
	TArray<FProcMeshTangent> Tangents;


	Mesh->GetVertices(Vertices, Normals, UVCoordinate, Tangents);
	Mesh->GetTriangles(Triangles);

	ProceduralMesh->CreateMeshSection(CurrentElement, Vertices, Triangles, Normals, UVCoordinate, TArray<FColor>(), Tangents, Collision);

	if (Mesh->GetMaterial())
	{
		ProceduralMesh->SetMaterial(CurrentElement, const_cast<UMaterialInterface*>(Mesh->GetMaterial()));
	}

	

	CurrentElement++;
}


void ALevelGenProceduralMeshActor::SetMaterial(int32 ElementIndex, UMaterialInterface* Material)
{
	ProceduralMesh->SetMaterial(ElementIndex, Material);
}