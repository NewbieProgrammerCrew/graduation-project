// Fill out your copyright notice in the Description page of Project Settings.


#include "MapEditorMode.h"

void AMapEditorMode::SetLadscapeMode()
{
	m_LandscapeMode = true;
}

void AMapEditorMode::SetTransformMode()
{
	m_LandscapeMode = false;
}

int AMapEditorMode::GetCurrentMode()
{
	return m_LandscapeMode ? 1 : 2;
}
