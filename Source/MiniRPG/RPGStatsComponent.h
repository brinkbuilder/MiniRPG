#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPGStatsComponent.generated.h"

// Reusable HP/Level/XP/Attack/Defense block -- attached to both the player
// and enemy pawns so combat code can treat them identically.
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class URPGStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URPGStatsComponent();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 Experience = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 ExperienceToNextLevel = 20;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 MaxHealth = 30;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 CurrentHealth = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 AttackPower = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 Defense = 1;

	void GainExperience(int32 Amount);
	void TakeDamage(int32 Amount);
	void Heal(int32 Amount);

	bool IsDead() const { return CurrentHealth <= 0; }

private:
	void LevelUp();
};
