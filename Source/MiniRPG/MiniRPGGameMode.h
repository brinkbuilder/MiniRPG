#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MiniRPGGameMode.generated.h"

// Spawns the ground, environment dressing, enemies, and pickups at
// BeginPlay, and tracks the win condition (all enemies defeated).
UCLASS()
class AMiniRPGGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMiniRPGGameMode();

	virtual void BeginPlay() override;

	void NotifyEnemyDefeated();

	UPROPERTY(EditDefaultsOnly, Category = "MiniRPG")
	TSubclassOf<AActor> EnemyClass;

	UPROPERTY(EditDefaultsOnly, Category = "MiniRPG")
	TSubclassOf<AActor> PickupClass;

	UPROPERTY(EditDefaultsOnly, Category = "MiniRPG")
	int32 NumberOfEnemies = 10;

	UPROPERTY(EditDefaultsOnly, Category = "MiniRPG")
	int32 NumberOfPickups = 7;

	UPROPERTY(EditDefaultsOnly, Category = "MiniRPG")
	float SpawnRadius = 2200.0f;

private:
	int32 EnemiesRemaining = 0;

	void SpawnGround();
	void SpawnEnvironment();
	void SpawnEnemies();
	void SpawnPickups();
};
