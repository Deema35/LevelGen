// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "EditorViewportClient.h"


class FLevelGenEditorViewportClient : public FEditorViewportClient
{
public:
	FLevelGenEditorViewportClient(SCustomEditorViewport * _CustomViewPort, FEditorModeTools* InModeTools, FPreviewScene* InPreviewScene = nullptr, const TWeakPtr<SEditorViewport>& InEditorViewportWidget = nullptr) :
		FEditorViewportClient(InModeTools, InPreviewScene, InEditorViewportWidget), CustomViewPort(_CustomViewPort){}

	virtual void Draw(FViewport* Viewport, FCanvas* Canvas) override;

	virtual bool InputKey(FViewport* Viewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed = 1.f, bool bGamepad = false) override;

private:

	SCustomEditorViewport * CustomViewPort;
};