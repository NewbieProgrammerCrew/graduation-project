// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInfo.h"

PlayerInfo::PlayerInfo()
{
}

PlayerInfo::~PlayerInfo() {}

void PlayerInfo::SetName(FString name) 
{
	m_name = name; 
}

void PlayerInfo::SetRole(std::string role) {
	m_role = role;
}

FString PlayerInfo::GetName() 
{
	return m_name;
 }

std::string PlayerInfo::GetRole() {
  return m_role;
}
