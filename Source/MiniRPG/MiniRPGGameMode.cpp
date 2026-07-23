#include "MiniRPGGameMode.h"
#include "MiniRPGPlayerPawn.h"
#include "MiniRPGEnemyPawn.h"
#include "MiniRPGPickup.h"
#include "Engine/Engine.h"

AMiniRPGGameMode::AMiniRPGGameMode()
{
	DefaultPawnClass = AMiniRPGPlayerPawn::StaticClass();
	EnemyClass = AMiniRPGEnemyPawn::StaticClass();
	PickupClass = AMiniRPGPickup::StaticClass();
}

void AMiniRPGGameMode::BeginPlay()
{
	Super::BeginPlay();
	SpawnEnemies();
	SpawnPickups();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Yellow,
			TEXT("WASD move, Space to attack nearby enemies, E to use a potion. Defeat enemies for XP!"));
	}
}

void AMiniRPGGameMode::SpawnEnemies()
{
	if (!GetWorld() || !EnemyClass)
	{
		return;
	}

	for (int32 i = 0; i < NumberOfEnemies; ++i)
	{
		const float Angle = FMath::DegreesToRadians(360.0f / NumberOfEnemies * i);
		const FVector Location(FMath::Cos(Angle) * SpawnRadius, FMath::Sin(Angle) * SpawnRadius, 50.0f);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<AActor>(EnemyClass, Location, FRotator::ZeroRotator, SpawnParams);
	}
}

void AMiniRPGGameMode::SpawnPickups()
{
	if (!GetWorld() || !PickupClass)
	{
		return;
	}

	for (int32 i = 0; i < NumberOfPickups; ++i)
	{
		const float Angle = FMath::DegreesToRadians(360.0f / NumberOfPickups * i) + 0.3f;
		const float Radius = SpawnRadius * 0.5f;
		const FVector Location(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 50.0f);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<AActor>(PickupClass, Location, FRotator::ZeroRotator, SpawnParams);
	}
}
