// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Interfaces/Interface_CollisionDataProvider.h"
#include "Components/MeshComponent.h"
#include "PhysicsEngine/ConvexElem.h"
#include <vector>
#include <memory>
#include "PrimitiveSceneProxy.h"
#include "DynamicMeshBuilder.h"
#include "LevelGenProceduralMeshComponent.generated.h"

class FProceduralFigureBase;
class ULevelGenProceduralMeshComponent;

/** Class representing a single section of the proc mesh */
class FLevelGenProcMeshProxySection
{
public:
	UMaterialInterface * Material = nullptr;

	FStaticMeshVertexBuffers VertexBuffers;

	FDynamicMeshIndexBuffer32 IndexBuffer;

	FLocalVertexFactory VertexFactory;

	bool bSectionVisible = true;

	FLevelGenProcMeshProxySection(ERHIFeatureLevel::Type InFeatureLevel)
		: VertexFactory(InFeatureLevel, "FLevelGenProcMeshProxySection")
		
	{}
};

struct FLevelGenProcMeshVertex
{
	FVector Position{0,0,0};

	FVector Normal{0,0,0};

	FVector Tangent{ 1,1,1 };

	FColor Color{ 255,255,255 };

	FVector2D UV0{ 0,0 };

};

struct FLevelGenProcMeshSection
{
	TArray<FLevelGenProcMeshVertex> ProcVertexBuffer;

	TArray<uint32> ProcIndexBuffer;

	FBox SectionLocalBox;

	bool bEnableCollision = false;

	bool bSectionVisible = true;

};



class FLevelGenProceduralMeshSceneProxy : public FPrimitiveSceneProxy
{
public:

	FLevelGenProceduralMeshSceneProxy(ULevelGenProceduralMeshComponent* Component);

	virtual ~FLevelGenProceduralMeshSceneProxy();

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily,
		uint32 VisibilityMap, FMeshElementCollector& Collector) const override;

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;

	virtual bool CanBeOccluded() const override { return !MaterialRelevance.bDisableDepthTest; }

	virtual uint32 GetMemoryFootprint(void) const override { return(sizeof(*this) + GetAllocatedSize()); }

	SIZE_T GetTypeHash() const override
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}

private:
	/** Array of sections */
	std::vector<std::shared_ptr<FLevelGenProcMeshProxySection>> Sections;

	UBodySetup* BodySetup;

	FMaterialRelevance MaterialRelevance;

};




UCLASS(hidecategories = (Object, LOD), meta = (BlueprintSpawnableComponent), ClassGroup = Rendering)
class ULevelGenProceduralMeshComponent : public UMeshComponent, public IInterface_CollisionDataProvider
{
	GENERATED_BODY()
public:
	ULevelGenProceduralMeshComponent() { bUseComplexAsSimpleCollision = true; }

	void CreateMeshSection(FProceduralFigureBase& Mesh);
	void UpdateMeshComponent();
	void ClearAllMeshSections();

	int32 GetNumSections() const { return ProcMeshSections.size(); }

	//~ Begin Interface_CollisionDataProvider Interface
	virtual bool GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData) override;
	virtual bool ContainsPhysicsTriMeshData(bool InUseAllTriData) const override;
	virtual bool WantsNegXTriMesh() override{ return false; }
	//~ End Interface_CollisionDataProvider Interface
	
	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual class UBodySetup* GetBodySetup() override;
	virtual UMaterialInterface* GetMaterialFromCollisionFaceIndex(int32 FaceIndex, int32& SectionIndex) const override;
	//~ End UPrimitiveComponent Interface.

	//~ Begin UMeshComponent Interface.
	virtual int32 GetNumMaterials() const override {return ProcMeshSections.size();}
	//~ End UMeshComponent Interface.

	//~ Begin UObject Interface
	virtual void PostLoad() override;
	//~ End UObject Interface.

	const std::vector<FLevelGenProcMeshSection>& GetProcMeshSections() const { return ProcMeshSections; }

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Procedural Mesh")
		bool bUseComplexAsSimpleCollision;
	/** Collision data */
	UPROPERTY(Instanced)
		class UBodySetup* ProcMeshBodySetup;

private:
	//~ Begin USceneComponent Interface.
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	//~ Begin USceneComponent Interface.

	/** Update LocalBounds member from the local box of each section */
	void UpdateLocalBounds();
	/** Mark collision data as dirty, and re-create on instance if necessary */
	void UpdateCollision();

	UBodySetup* CreateBodySetupHelper();

private:

	std::vector<FLevelGenProcMeshSection> ProcMeshSections;

	TArray<FKConvexElem> CollisionConvexElems;

	FBoxSphereBounds LocalBounds;

};


