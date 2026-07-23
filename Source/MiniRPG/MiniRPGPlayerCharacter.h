#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MiniRPGPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class URPGStatsComponent;
class UInventoryComponent;
class AMiniRPGEnemyCharacter;
class AMiniRPGProp;

// A gathering skill's own level/XP track (Woodcutting, Mining) -- separate
// from combat level, RuneScape-style.
USTRUCT()
struct FGatherSkill
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Skill")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, Category = "Skill")
	int32 XP = 0;

	UPROPERTY(VisibleAnywhere, Category = "Skill")
	int32 XPToNextLevel = 30;

	void GainXP(int32 Amount)
	{
		XP += Amount;
		while (XP >= XPToNextLevel)
		{
			XP -= XPToNextLevel;
			Level++;
			XPToNextLevel = FMath::RoundToInt(XPToNextLevel * 1.35f);
		}
	}
};

// The player-controlled character: a real animated humanoid with a follow
// camera. Primary control is point-and-click (RuneScape-style) -- click
// the ground to walk, an enemy to auto-walk-and-fight it, or a tree/rock
// to auto-walk-and-gather it. WASD/Space/E still work as a manual fallback.
UCLASS()
class AMiniRPGPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMiniRPGPlayerCharacter();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	URPGStatsComponent* StatsComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UInventoryComponent* InventoryComponent;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackRange = 220.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackCooldown = 0.6f;

	UPROPERTY(EditAnywhere, Category = "Gathering")
	float GatherRange = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Gathering")
	float GatherCooldown = 2.0f;

	UPROPERTY(VisibleAnywhere, Category = "Gathering")
	FGatherSkill WoodcuttingSkill;

	UPROPERTY(VisibleAnywhere, Category = "Gathering")
	FGatherSkill MiningSkill;

private:
	bool bHasMoveTarget = false;
	FVector MoveTargetLocation = FVector::ZeroVector;
	TWeakObjectPtr<AMiniRPGEnemyCharacter> AttackTarget;
	TWeakObjectPtr<AMiniRPGProp> GatherTarget;

	float LastAttackTime = -10.0f;
	float LastGatherTime = -10.0f;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void OnAttackPressed();
	void OnUseItemPressed();
	void OnClickPressed();

	void UpdateAutoActions();
	void MoveToward(const FVector& TargetLocation);
	void CancelAutoAction();
	void DealDamageToEnemy(AMiniRPGEnemyCharacter* Enemy);
	void TryGather(AMiniRPGProp* Prop);
};
