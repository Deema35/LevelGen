// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved. 

#include "LevelGenProceduralMeshComponent.h"
#include "PrimitiveViewRelevance.h"
#include "RenderResource.h"
#include "RenderingThread.h"
#include "Containers/ResourceArray.h"
#include "EngineGlobals.h"
#include "VertexFactory.h"
#include "MaterialShared.h"
#include "Materials/Material.h"
#include "LocalVertexFactory.h"
#include "Engine/Engine.h"
#include "SceneManagement.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "StaticMeshResources.h"
#include "ProceduralFigure.h"

static void ConvertProcMeshToDynMeshVertex(FDynamicMeshVertex& Vert, const FLevelGenProcMeshVertex& ProcVert)
{
	Vert.Position = ProcVert.Position;
	Vert.Color = ProcVert.Color;
	Vert.TextureCoordinate[0] = ProcVert.UV0;
	Vert.TangentX = ProcVert.Tangent;
	Vert.TangentZ = ProcVert.Normal;
	Vert.TangentZ.Vector.W =  255;
}

FLevelGenProceduralMeshSceneProxy::FLevelGenProceduralMeshSceneProxy(ULevelGenProceduralMeshComponent* Component)
	: FPrimitiveSceneProxy(Component)
	, BodySetup(Component->GetBodySetup())
	, MaterialRelevance(Component->GetMaterialRelevance(GetScene().GetFeatureLevel()))
{
	// Copy each section
	
	const std::vector<FLevelGenProcMeshSection>& ComponentSections = Component->GetProcMeshSections();
	Sections.reserve(ComponentSections.size());

	for (int SectionIdx = 0; SectionIdx < ComponentSections.size(); SectionIdx++)
	{
		const FLevelGenProcMeshSection& SrcSection = ComponentSections[SectionIdx];

		if (SrcSection.ProcIndexBuffer.Num() > 3 && SrcSection.ProcVertexBuffer.Num() > 0)
		{
			std::shared_ptr<FLevelGenProcMeshProxySection> NewSection(new FLevelGenProcMeshProxySection(GetScene().GetFeatureLevel()));
			Sections.push_back(NewSection);

			// Allocate verts

			TArray<FDynamicMeshVertex> Vertices;

			Vertices.SetNumUninitialized(SrcSection.ProcVertexBuffer.Num());
			int VertIdx = 0;
			// Copy verts
			for (int i = 0; i < SrcSection.ProcVertexBuffer.Num(); i++)
			{
				FDynamicMeshVertex& Vert = Vertices[VertIdx++];
				ConvertProcMeshToDynMeshVertex(Vert, SrcSection.ProcVertexBuffer[i]);
				
				
			}

		
			// Copy index buffer
			NewSection->IndexBuffer.Indices = SrcSection.ProcIndexBuffer;

			NewSection->VertexBuffers.InitFromDynamicVertex(&NewSection->VertexFactory, Vertices);

			// Enqueue initialization of render resource
			BeginInitResource(&NewSection->VertexBuffers.PositionVertexBuffer);
			BeginInitResource(&NewSection->VertexBuffers.StaticMeshVertexBuffer);
			BeginInitResource(&NewSection->VertexBuffers.ColorVertexBuffer);
			BeginInitResource(&NewSection->IndexBuffer);
			BeginInitResource(&NewSection->VertexFactory);

			// Grab material
			NewSection->Material = Component->GetMaterial(SectionIdx);
			if (NewSection->Material == NULL)
			{
				NewSection->Material = UMaterial::GetDefaultMaterial(MD_Surface);
			}

			// Copy visibility info
			NewSection->bSectionVisible = true;

		}
	}
}

FLevelGenProceduralMeshSceneProxy::~FLevelGenProceduralMeshSceneProxy()
{
	for (int i = 0; i <  Sections.size(); i++)
	{
		if (Sections[i])
		{
			Sections[i]->VertexBuffers.PositionVertexBuffer.ReleaseResource();
			Sections[i]->VertexBuffers.StaticMeshVertexBuffer.ReleaseResource();
			Sections[i]->VertexBuffers.ColorVertexBuffer.ReleaseResource();
			Sections[i]->IndexBuffer.ReleaseResource();
			Sections[i]->VertexFactory.ReleaseResource();
		}
	}
}

	

void FLevelGenProceduralMeshSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap,
	FMeshElementCollector& Collector) const 
{

	// Set up wireframe material (if needed)
	const bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;

	FColoredMaterialRenderProxy* WireframeMaterialInstance = NULL;
	if (bWireframe)
	{
		WireframeMaterialInstance = new FColoredMaterialRenderProxy(
			GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy(IsSelected()) : NULL,
			FLinearColor(0, 0.5f, 1.f)
		);

		Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);
	}

	// Iterate over sections
	for (int i = 0; i < Sections.size(); i++)
	{
		if (Sections[i] && Sections[i]->bSectionVisible)
		{
			FMaterialRenderProxy* MaterialProxy = bWireframe ? WireframeMaterialInstance : Sections[i]->Material->GetRenderProxy(IsSelected());

			// For each view..
			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];
					// Draw the mesh.
					FMeshBatch& Mesh = Collector.AllocateMesh();
					FMeshBatchElement& BatchElement = Mesh.Elements[0];
					BatchElement.IndexBuffer = &Sections[i]->IndexBuffer;
					Mesh.bWireframe = bWireframe;
					Mesh.VertexFactory = &Sections[i]->VertexFactory;
					Mesh.MaterialRenderProxy = MaterialProxy;
					BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true, UseEditorDepthTest());
					BatchElement.FirstIndex = 0;
					BatchElement.NumPrimitives = Sections[i]->IndexBuffer.Indices.Num() / 3;
					BatchElement.MinVertexIndex = 0;
					BatchElement.MaxVertexIndex = Sections[i]->VertexBuffers.PositionVertexBuffer.GetNumVertices() - 1;
					Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
					Mesh.Type = PT_TriangleList;
					Mesh.DepthPriorityGroup = SDPG_World;
					Mesh.bCanApplyViewModeOverrides = false;
					Collector.AddMesh(ViewIndex, Mesh);
				}
			}
		}
	}

	// Draw bounds
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		if (VisibilityMap & (1 << ViewIndex))
		{
			// Draw simple collision as wireframe if 'show collision', and collision is enabled, and we are not using the complex as the simple
			if (ViewFamily.EngineShowFlags.Collision && IsCollisionEnabled() && BodySetup->GetCollisionTraceFlag() != ECollisionTraceFlag::CTF_UseComplexAsSimple)
			{
				FTransform GeomTransform(GetLocalToWorld());
				BodySetup->AggGeom.GetAggGeom(GeomTransform, GetSelectionColor(FColor(157, 149, 223, 255), IsSelected(), IsHovered()).ToFColor(true), NULL, false, false, UseEditorDepthTest(), ViewIndex, Collector);
			}

			// Render bounds
			RenderBounds(Collector.GetPDI(ViewIndex), ViewFamily.EngineShowFlags, GetBounds(), IsSelected());
		}
	}
#endif
}

FPrimitiveViewRelevance FLevelGenProceduralMeshSceneProxy::GetViewRelevance(const FSceneView* View) const
{
	FPrimitiveViewRelevance Result;
	Result.bDrawRelevance = IsShown(View);
	Result.bShadowRelevance = IsShadowCast(View);
	Result.bDynamicRelevance = true;
	Result.bRenderInMainPass = ShouldRenderInMainPass();
	Result.bUsesLightingChannels = GetLightingChannelMask() != GetDefaultLightingChannelMask();
	Result.bRenderCustomDepth = ShouldRenderCustomDepth();
	MaterialRelevance.SetPrimitiveViewRelevance(Result);
	return Result;
}

	

	

//*********************************
//ULevelGenProceduralMeshComponent
//***********************************


void ULevelGenProceduralMeshComponent::PostLoad()
{
	Super::PostLoad();

	if (ProcMeshBodySetup && IsTemplate())
	{
		ProcMeshBodySetup->SetFlags(RF_Public);
	}
}


void ULevelGenProceduralMeshComponent::CreateMeshSection(FProceduralFigureBase& Mesh)
{
	ProcMeshSections.push_back(FLevelGenProcMeshSection());
	Mesh.GetLevelGenMeshSection(ProcMeshSections.back());
	
}

void ULevelGenProceduralMeshComponent::UpdateMeshComponent()
{
	UpdateLocalBounds();
	UpdateCollision();
	MarkRenderStateDirty();
}



void ULevelGenProceduralMeshComponent::ClearAllMeshSections()
{
	ProcMeshSections.clear();
	UpdateLocalBounds();
	UpdateCollision();
	MarkRenderStateDirty();
}



void ULevelGenProceduralMeshComponent::UpdateLocalBounds()
{
	FBox LocalBox(ForceInit);

	for (int i = 0; i < ProcMeshSections.size(); i++)
	{
		LocalBox += ProcMeshSections[i].SectionLocalBox;
	}

	LocalBounds = LocalBox.IsValid ? FBoxSphereBounds(LocalBox) : FBoxSphereBounds(FVector(0, 0, 0), FVector(0, 0, 0), 0); // fallback to reset box sphere bounds

	// Update global bounds
	UpdateBounds();
	// Need to send to render thread
	MarkRenderTransformDirty();
}

FPrimitiveSceneProxy* ULevelGenProceduralMeshComponent::CreateSceneProxy()
{
	return new FLevelGenProceduralMeshSceneProxy(this);
}


FBoxSphereBounds ULevelGenProceduralMeshComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	FBoxSphereBounds Ret(LocalBounds.TransformBy(LocalToWorld));

	Ret.BoxExtent *= BoundsScale;
	Ret.SphereRadius *= BoundsScale;

	return Ret;
}

bool ULevelGenProceduralMeshComponent::GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData)
{
	int32 VertexBase = 0; // Base vertex index for current section

	// See if we should copy UVs
	bool bCopyUVs = UPhysicsSettings::Get()->bSupportUVFromHitResults; 
	if (bCopyUVs)
	{
		CollisionData->UVs.AddZeroed(1); // only one UV channel
	}

	// For each section..
	for (int32 SectionIdx = 0; SectionIdx < ProcMeshSections.size(); SectionIdx++)
	{
		FLevelGenProcMeshSection& Section = ProcMeshSections[SectionIdx];
		// Do we have collision enabled?
		if (Section.bEnableCollision)
		{
			// Copy vert data
			for (int i = 0; i < Section.ProcVertexBuffer.Num(); i++)
			{
				CollisionData->Vertices.Add(Section.ProcVertexBuffer[i].Position);

				// Copy UV if desired
				if (bCopyUVs)
				{
					CollisionData->UVs[0].Add(Section.ProcVertexBuffer[i].UV0);
				}
			}
			
			// Copy triangle data

			const int32 NumTriangles = Section.ProcIndexBuffer.Num() / 3;

			for (int32 TriIdx = 0; TriIdx < NumTriangles; TriIdx++)
			{
				// Need to add base offset for indices
				FTriIndices Triangle;
				Triangle.v0 = Section.ProcIndexBuffer[(TriIdx * 3) + 0] + VertexBase;
				Triangle.v1 = Section.ProcIndexBuffer[(TriIdx * 3) + 1] + VertexBase;
				Triangle.v2 = Section.ProcIndexBuffer[(TriIdx * 3) + 2] + VertexBase;
				CollisionData->Indices.Add(Triangle);

				// Also store material info
				CollisionData->MaterialIndices.Add(SectionIdx);
			}

			// Remember the base index that new verts will be added from in next section
			VertexBase = CollisionData->Vertices.Num();
		}
	}

	CollisionData->bFlipNormals = true;
	CollisionData->bDeformableMesh = true;
	CollisionData->bFastCook = true;

	return true;
}

bool ULevelGenProceduralMeshComponent::ContainsPhysicsTriMeshData(bool InUseAllTriData) const
{
	for (int i = 0; i < ProcMeshSections.size(); i++)
	{
		if (ProcMeshSections[i].ProcIndexBuffer.Num() >= 3 && ProcMeshSections[i].bEnableCollision)
		{
			return true;
		}
	}

	return false;
}

UBodySetup* ULevelGenProceduralMeshComponent::CreateBodySetupHelper()
{
	// The body setup in a template needs to be public since the property is Tnstanced and thus is the archetype of the instance meaning there is a direct reference
	UBodySetup* NewBodySetup = NewObject<UBodySetup>(this, NAME_None, (IsTemplate() ? RF_Public : RF_NoFlags));
	NewBodySetup->BodySetupGuid = FGuid::NewGuid();

	NewBodySetup->bGenerateMirroredCollision = false;
	NewBodySetup->bDoubleSidedGeometry = true;
	NewBodySetup->CollisionTraceFlag = bUseComplexAsSimpleCollision ? CTF_UseComplexAsSimple : CTF_UseDefault;

	return NewBodySetup;
}



void ULevelGenProceduralMeshComponent::UpdateCollision()
{

	UWorld* World = GetWorld();

	if (ProcMeshBodySetup == nullptr)
	{
		ProcMeshBodySetup = CreateBodySetupHelper();
	}
	
	
	UBodySetup* UseBodySetup =  ProcMeshBodySetup;

	// Fill in simple collision convex elements
	UseBodySetup->AggGeom.ConvexElems = CollisionConvexElems;

	// Set trace flag
	UseBodySetup->CollisionTraceFlag = bUseComplexAsSimpleCollision ? CTF_UseComplexAsSimple : CTF_UseDefault;

	
	// New GUID as collision has changed
	UseBodySetup->BodySetupGuid = FGuid::NewGuid();
	// Also we want cooked data for this
	UseBodySetup->bHasCookedCollisionData = true;
	UseBodySetup->InvalidatePhysicsData();
	UseBodySetup->CreatePhysicsMeshes();
	RecreatePhysicsState();
	
}


UBodySetup* ULevelGenProceduralMeshComponent::GetBodySetup()
{
	if (ProcMeshBodySetup == nullptr)
	{
		ProcMeshBodySetup = CreateBodySetupHelper();
	}
	return ProcMeshBodySetup;
}

UMaterialInterface* ULevelGenProceduralMeshComponent::GetMaterialFromCollisionFaceIndex(int32 FaceIndex, int32& SectionIndex) const
{
	UMaterialInterface* Result = nullptr;
	SectionIndex = 0;

	if (FaceIndex >= 0)
	{
		// Look for element that corresponds to the supplied face
		int32 TotalFaceCount = 0;
		for (int32 SectionIdx = 0; SectionIdx < ProcMeshSections.size(); SectionIdx++)
		{
			int32 NumFaces = ProcMeshSections[SectionIdx].ProcIndexBuffer.Num() / 3;
			TotalFaceCount += NumFaces;

			if (FaceIndex < TotalFaceCount)
			{
				// Grab the material
				Result = GetMaterial(SectionIdx);
				SectionIndex = SectionIdx;
				break;
			}
		}
	}

	return Result;
}