// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class NPC_WORLD_API PlayerInfo
{
public:
	PlayerInfo();
	~PlayerInfo();
	void SetName(FString name);
	void SetRole(std::string role);
	FString GetName();
	std::string GetRole();
private:
	FString m_name;
	std::string m_role;

};
