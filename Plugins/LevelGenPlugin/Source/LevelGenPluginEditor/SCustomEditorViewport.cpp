// Copyright 2018 Pavlov Dmitriy
#include "SCustomEditorViewport.h"
#include "EditorStyleSet.h"
#include "EditorViewportClient.h"
#include "SDockTab.h"
#include "PreviewScene.h"
#include "Components/SkyLightComponent.h"
#include "Atmosphere/AtmosphericFogComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Widgets/Input/SButton.h"
#include "EngineUtils.h"
#include "LevelGenEditorViewportClient.h"

void SCustomEditorViewport::Construct(const FArguments& InArgs)
{
	MakePreviewScene();
}

void SCustomEditorViewport::MakePreviewScene()
{
	PreviewScene = MakeShareable(new FPreviewScene);
	SEditorViewport::Construct(SEditorViewport::FArguments());
	Skylight = NewObject<USkyLightComponent>();
	PreviewScene->AddComponent(Skylight, FTransform::Identity);
	AtmosphericFog = NewObject<UAtmosphericFogComponent>();
	PreviewScene->AddComponent(AtmosphericFog, FTransform::Identity);
	FString Name = "ViewPort";

	PreviewScene->DirectionalLight->CastShadows = true;
	PreviewScene->DirectionalLight->CastStaticShadows = true;
	PreviewScene->DirectionalLight->CastDynamicShadows = true;
}

EVisibility SCustomEditorViewport::GetToolbarVisibility() const
{
	return EVisibility::Visible;
}

TSharedRef<FEditorViewportClient> SCustomEditorViewport::MakeEditorViewportClient()
{

	FPreviewScene& pInPreviewScene = *PreviewScene;
	EditorViewportClient = MakeShareable(new FLevelGenEditorViewportClient(this, nullptr, &pInPreviewScene));
	EditorViewportClient->bSetListenerPosition = false;
	EditorViewportClient->SetRealtime(true);	// TODO: Check if real-time is needed
	EditorViewportClient->VisibilityDelegate.BindSP(this, &SCustomEditorViewport::IsVisible);
	
	// Set the initial camera position
	FRotator OrbitRotation(-40, 10, 0);
	EditorViewportClient->SetCameraSetup(
		FVector::ZeroVector,
		OrbitRotation,
		FVector(0.0f, 100, 0.0f),
		FVector::ZeroVector,
		FVector(300, 400, 500),
		FRotator(-10, 200, 0)
		);
	EditorViewportClient->SetViewLocation(FVector(500, 300, 500));

	EditorViewportClient->EngineShowFlags.SetSnap(0);
	EditorViewportClient->EngineShowFlags.CompositeEditorPrimitives = true;
	EditorViewportClient->bUsingOrbitCamera = true;
	EditorViewportClient->SetViewMode(EViewModeIndex::VMI_Lit);
	return EditorViewportClient.ToSharedRef();
}

EVisibility SCustomEditorViewport::OnGetViewportContentVisibility() const
{
	return EVisibility::Visible;
}

bool SCustomEditorViewport::IsVisible() const
{
	return ViewportWidget.IsValid() && (!ParentTab.IsValid() || ParentTab.Pin()->IsForeground());
}


void SCustomEditorViewport::OnFocusViewportToSelection()
{
	SEditorViewport::OnFocusViewportToSelection();
}

void SCustomEditorViewport::RebildScen(UClass* ActorClass)
{
	DestroyActors();

	if (ActorClass)PreviewScene->GetWorld()->SpawnActor<AActor>(ActorClass, FVector::ZeroVector, FRotator::ZeroRotator);
	
}

void SCustomEditorViewport::DestroyActors()
{
	for (TActorIterator<AActor> ActorItr(PreviewScene->GetWorld()); ActorItr; ++ActorItr)
	{
		
		ActorItr->Destroy();
		
	}
}

bool SCustomEditorViewport::ChackScenBeforeDraw()
{
	if (!PreviewScene->GetWorld()->GetWorldSettings(false, false))
	{
		IsViewPortValid = false;
		return false;
	}
	return true;
}

