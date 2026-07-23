#include "MiniRPGEnemyPawn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "RPGStatsComponent.h"
#include "Kismet/GameplayStatics.h"

AMiniRPGEnemyPawn::AMiniRPGEnemyPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(50.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("Pawn"));
	RootComponent = CollisionComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMeshAsset(TEXT("/Engine/BasicShapes/Cone.Cone"));
	if (ConeMeshAsset.Succeeded())
	{
		MeshComponent->SetStaticMesh(ConeMeshAsset.Object);
	}

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
	MovementComponent->MaxSpeed = 350.0f;
	MovementComponent->Acceleration = 2000.0f;
	MovementComponent->Deceleration = 2000.0f;

	StatsComponent = CreateDefaultSubobject<URPGStatsComponent>(TEXT("StatsComponent"));
	StatsComponent->MaxHealth = 20;
	StatsComponent->AttackPower = 3;
	StatsComponent->Defense = 0;
}

void AMiniRPGEnemyPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!StatsComponent || StatsComponent->IsDead())
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn)
	{
		return;
	}

	URPGStatsComponent* PlayerStats = PlayerPawn->FindComponentByClass<URPGStatsComponent>();
	if (!PlayerStats || PlayerStats->IsDead())
	{
		return;
	}

	const float Distance = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());

	if (Distance <= DetectRange && Distance > AttackRange)
	{
		const FVector Direction = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		AddMovementInput(Direction, 1.0f);
		SetActorRotation(Direction.Rotation());
	}
	else if (Distance <= AttackRange)
	{
		const float Now = GetWorld()->GetTimeSeconds();
		if (Now - LastAttackTime >= AttackCooldown)
		{
			LastAttackTime = Now;
			PlayerStats->TakeDamage(StatsComponent->AttackPower);
		}
	}
}
