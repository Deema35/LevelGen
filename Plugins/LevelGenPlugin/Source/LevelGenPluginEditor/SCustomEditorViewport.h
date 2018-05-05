// Copyright 2018 Pavlov Dmitriy
#pragma once

#include "SEditorViewport.h"

class FActorStorage_AssetEditorBase;
class UAtmosphericFogComponent;
class USkyLightComponent;
class FPreviewScene;


class SCustomEditorViewport : public SEditorViewport
{
public:
	SLATE_BEGIN_ARGS(SCustomEditorViewport){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);


	EVisibility GetToolbarVisibility() const;
	void RebildScen(UClass* ActorClass);

	void SetParentTab(TSharedPtr<SDockTab> InParentTab) { ParentTab = InParentTab; }

	bool ChackScenBeforeDraw();

	bool GetIsViewPortValid() const { return IsViewPortValid; }

protected:
	/** SEditorViewport interface */
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	virtual EVisibility OnGetViewportContentVisibility() const override;
	virtual void OnFocusViewportToSelection() override;
	
private:

	void DestroyActors();
	bool IsVisible() const;
	void MakePreviewScene();

private:
	TSharedPtr<class FEditorViewportClient> EditorViewportClient;
	TSharedPtr<FPreviewScene> PreviewScene;
	USkyLightComponent* Skylight;
	TWeakPtr<SDockTab> ParentTab;
	UAtmosphericFogComponent* AtmosphericFog;
	bool IsViewPortValid = true;
};
