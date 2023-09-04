// Fill out your copyright notice in the Description page of Project Settings.


#include "FilePickerWidget.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFileManager.h"
#include "DesktopPlatform/Public/DesktopPlatformModule.h"

void UFilePickerWidget::OpenFilePicker()
{
	TArray<FString> OutFiles;
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (DesktopPlatform)
	{
		uint32 SelectionFlag = 0;
		DesktopPlatform->OpenFileDialog(
			NULL,
			TEXT("Open"),
			TEXT(""),
			TEXT(""),
			TEXT("User Map files (*.uNPC)|*.uNPC"),
			SelectionFlag,
			OutFiles
		);

	}
	if (OutFiles.Num() > 0) {
		SelectFilePath = OutFiles[0];
		FileParser = NewObject<UMapFileParser>(this);
		FileParser->ParseMapFile(SelectFilePath);
	}
}