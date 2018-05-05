// Copyright 2018 Pavlov Dmitriy
#include "LevelGenEditorViewportClient.h"
#include "SCustomEditorViewport.h"

void FLevelGenEditorViewportClient::Draw(FViewport* Viewport, FCanvas* Canvas)
{

	if (CustomViewPort->ChackScenBeforeDraw()) FEditorViewportClient::Draw(Viewport, Canvas);
	
}

bool FLevelGenEditorViewportClient::InputKey(FViewport* Viewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed, bool bGamepad)
{
	if (CustomViewPort->ChackScenBeforeDraw()) return FEditorViewportClient::InputKey(Viewport, ControllerId,  Key, Event, AmountDepressed,  bGamepad);

	return false;
}