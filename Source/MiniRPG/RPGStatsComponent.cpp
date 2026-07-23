#include "RPGStatsComponent.h"

URPGStatsComponent::URPGStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URPGStatsComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
}

void URPGStatsComponent::GainExperience(int32 Amount)
{
	Experience += Amount;
	while (Experience >= ExperienceToNextLevel)
	{
		Experience -= ExperienceToNextLevel;
		LevelUp();
	}
}

void URPGStatsComponent::LevelUp()
{
	Level++;
	MaxHealth += 10;
	CurrentHealth = MaxHealth;
	AttackPower += 2;
	Defense += 1;
	ExperienceToNextLevel = FMath::RoundToInt(ExperienceToNextLevel * 1.4f);
}

void URPGStatsComponent::TakeDamage(int32 Amount)
{
	const int32 Reduced = FMath::Max(1, Amount - Defense);
	CurrentHealth = FMath::Max(0, CurrentHealth - Reduced);
}

void URPGStatsComponent::Heal(int32 Amount)
{
	CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + Amount);
}
