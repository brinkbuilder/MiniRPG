#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MiniRPGEnemyCharacter.generated.h"

class URPGStatsComponent;

// A simple enemy: chases the player character once in range, attacks on a
// cooldown once adjacent. No AIController/behavior tree -- just Tick.
// Same animated humanoid mesh as the player, tinted a different color.
UCLASS()
class AMiniRPGEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMiniRPGEnemyCharacter();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	URPGStatsComponent* StatsComponent;

	UPROPERTY(EditAnywhere, Category = "AI")
	float DetectRange = 900.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float AttackRange = 180.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float AttackCooldown = 1.2f;

	UPROPERTY(EditAnywhere, Category = "AI")
	int32 ExperienceReward = 10;

private:
	float LastAttackTime = -10.0f;
};
