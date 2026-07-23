#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MiniRPGEnemyPawn.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UFloatingPawnMovement;
class URPGStatsComponent;

// A simple enemy: chases the player pawn once in range, attacks on a
// cooldown once adjacent. No AIController/behavior tree -- just Tick.
UCLASS()
class AMiniRPGEnemyPawn : public APawn
{
	GENERATED_BODY()

public:
	AMiniRPGEnemyPawn();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UFloatingPawnMovement* MovementComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	URPGStatsComponent* StatsComponent;

	UPROPERTY(EditAnywhere, Category = "AI")
	float DetectRange = 700.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float AttackRange = 150.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float AttackCooldown = 1.2f;

	UPROPERTY(EditAnywhere, Category = "AI")
	int32 ExperienceReward = 10;

private:
	float LastAttackTime = -10.0f;
};
