// Fill out your copyright notice in the Description page of Project Settings.
#include "ShieldDodgeCombo.h"
#include "Shield.h"
#include "SevenCharacter.h"


void UShieldDodgeCombo::ComboAttackStart()
{
	Attacker->EquippedShield->EnableShieldHits(true);
}

void UShieldDodgeCombo::ComboAttackEnd()
{
	Attacker->EquippedShield->EnableShieldHits(false);
}