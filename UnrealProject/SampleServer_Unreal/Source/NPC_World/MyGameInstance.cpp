// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

UMyGameInstance::UMyGameInstance() { 
	m_playerInfo = new PlayerInfo(); 

}

void UMyGameInstance::SetRole(FString role) {
  const TCHAR *ch = *role;
  std::wstring ws{ch};
    m_playerInfo->SetRole(std::string(ws.begin(),ws.end()));
  }

std::string UMyGameInstance::GetRole() { return m_playerInfo->GetRole(); }



