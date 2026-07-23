#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MiniRPGGameMode.generated.h"

// Spawns enemies and pickups in rings around the start point at BeginPlay.
UCLASS()
class AMiniRPGGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMiniRPGGameMode();

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "MiniRPG")
	TSubclassOf<AActor> EnemyClass;

	UPROPERTY(EditDefaultsOnly, Category = "MiniRPG")
	TSubclassOf<AActor> PickupClass;

	UPROPERTY(EditDefaultsOnly, Category = "MiniRPG")
	int32 NumberOfEnemies = 4;

	UPROPERTY(EditDefaultsOnly, Category = "MiniRPG")
	int32 NumberOfPickups = 4;

	UPROPERTY(EditDefaultsOnly, Category = "MiniRPG")
	float SpawnRadius = 900.0f;

private:
	void SpawnEnemies();
	void SpawnPickups();
};
