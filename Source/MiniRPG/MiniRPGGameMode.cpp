#include "MiniRPGGameMode.h"
#include "MiniRPGPlayerCharacter.h"
#include "MiniRPGEnemyCharacter.h"
#include "MiniRPGPickup.h"
#include "MiniRPGProp.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"

AMiniRPGGameMode::AMiniRPGGameMode()
{
	DefaultPawnClass = AMiniRPGPlayerCharacter::StaticClass();
	EnemyClass = AMiniRPGEnemyCharacter::StaticClass();
	PickupClass = AMiniRPGPickup::StaticClass();
}

void AMiniRPGGameMode::BeginPlay()
{
	Super::BeginPlay();

	SpawnGround();
	SpawnEnvironment();
	SpawnEnemies();
	SpawnPickups();
	EnemiesRemaining = NumberOfEnemies;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 7.0f, FColor::Yellow,
			TEXT("Explore the clearing, defeat all raiders, collect potions and a weapon upgrade. WASD move, Space attack, E use item."));
	}
}

void AMiniRPGGameMode::SpawnGround()
{
	UStaticMesh* PlaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (!GetWorld() || !PlaneMesh)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AStaticMeshActor* Ground = GetWorld()->SpawnActor<AStaticMeshActor>(FVector(0.0f, 0.0f, 2.0f), FRotator::ZeroRotator, Params);
	if (!Ground)
	{
		return;
	}

	UStaticMeshComponent* GroundMesh = Ground->GetStaticMeshComponent();
	GroundMesh->SetMobility(EComponentMobility::Movable);
	GroundMesh->SetStaticMesh(PlaneMesh);
	Ground->SetActorScale3D(FVector(SpawnRadius / 50.0f));

	if (UMaterialInstanceDynamic* Dyn = GroundMesh->CreateAndSetMaterialInstanceDynamic(0))
	{
		Dyn->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.14f, 0.30f, 0.13f));
	}
}

void AMiniRPGGameMode::SpawnEnvironment()
{
	if (!GetWorld())
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const int32 NumTrees = 26;
	for (int32 i = 0; i < NumTrees; ++i)
	{
		const float Angle = FMath::DegreesToRadians(FMath::FRandRange(0.0f, 360.0f));
		const float Radius = FMath::FRandRange(SpawnRadius * 0.65f, SpawnRadius * 1.25f);
		const FVector Location(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 0.0f);

		if (AMiniRPGProp* Tree = GetWorld()->SpawnActor<AMiniRPGProp>(Location, FRotator::ZeroRotator, Params))
		{
			Tree->ConfigureAsTree(FMath::FRandRange(2.5f, 4.5f),
				FLinearColor(0.28f, 0.17f, 0.08f), FLinearColor(0.08f, 0.32f, 0.11f));
		}
	}

	const int32 NumRocks = 16;
	for (int32 i = 0; i < NumRocks; ++i)
	{
		const float Angle = FMath::DegreesToRadians(FMath::FRandRange(0.0f, 360.0f));
		const float Radius = FMath::FRandRange(SpawnRadius * 0.25f, SpawnRadius * 1.1f);
		const FVector Location(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 20.0f);

		if (AMiniRPGProp* Rock = GetWorld()->SpawnActor<AMiniRPGProp>(Location, FRotator::ZeroRotator, Params))
		{
			Rock->ConfigureAsRock(FMath::FRandRange(0.4f, 1.1f), FLinearColor(0.33f, 0.33f, 0.35f));
		}
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
		const float Radius = FMath::FRandRange(SpawnRadius * 0.35f, SpawnRadius * 0.85f);
		const FVector Location(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 50.0f);

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
		const float Angle = FMath::DegreesToRadians(360.0f / NumberOfPickups * i + 15.0f);
		const float Radius = FMath::FRandRange(SpawnRadius * 0.15f, SpawnRadius * 0.6f);
		const FVector Location(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 50.0f);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AActor* SpawnedPickup = GetWorld()->SpawnActor<AActor>(PickupClass, Location, FRotator::ZeroRotator, SpawnParams);

		if (AMiniRPGPickup* Pickup = Cast<AMiniRPGPickup>(SpawnedPickup))
		{
			// Last pickup in the ring is the weapon upgrade; the rest are potions.
			if (i == NumberOfPickups - 1)
			{
				Pickup->ConfigurePickup(TEXT("Weapon Upgrade"), EItemType::WeaponUpgrade, 4);
			}
			else
			{
				Pickup->ConfigurePickup(TEXT("Health Potion"), EItemType::Potion, 15);
			}
		}
	}
}

void AMiniRPGGameMode::NotifyEnemyDefeated()
{
	EnemiesRemaining = FMath::Max(0, EnemiesRemaining - 1);

	if (EnemiesRemaining == 0 && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan,
			TEXT("Victory! You've cleared the clearing of every raider."));
	}
}
