#include "MiniRPGPlayerPawn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "RPGStatsComponent.h"
#include "InventoryComponent.h"
#include "MiniRPGEnemyPawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/EngineTypes.h"
#include "Engine/Engine.h"

AMiniRPGPlayerPawn::AMiniRPGPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(50.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("Pawn"));
	RootComponent = CollisionComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMeshAsset.Succeeded())
	{
		MeshComponent->SetStaticMesh(SphereMeshAsset.Object);
	}

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 700.0f;
	SpringArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	SpringArm->bDoCollisionTest = false;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 6.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
	MovementComponent->MaxSpeed = 700.0f;
	MovementComponent->Acceleration = 4000.0f;
	MovementComponent->Deceleration = 4000.0f;

	StatsComponent = CreateDefaultSubobject<URPGStatsComponent>(TEXT("StatsComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void AMiniRPGPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMiniRPGPlayerPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMiniRPGPlayerPawn::MoveRight);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AMiniRPGPlayerPawn::OnAttackPressed);
	PlayerInputComponent->BindAction("UseItem", IE_Pressed, this, &AMiniRPGPlayerPawn::OnUseItemPressed);
}

void AMiniRPGPlayerPawn::MoveForward(float Value)
{
	AddMovementInput(FVector::ForwardVector, Value);
}

void AMiniRPGPlayerPawn::MoveRight(float Value)
{
	AddMovementInput(FVector::RightVector, Value);
}

void AMiniRPGPlayerPawn::OnAttackPressed()
{
	if (!StatsComponent || StatsComponent->IsDead())
	{
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastAttackTime < AttackCooldown)
	{
		return;
	}
	LastAttackTime = Now;

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	TArray<AActor*> OverlappingActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	UKismetSystemLibrary::SphereOverlapActors(this, GetActorLocation(), AttackRange, ObjectTypes,
		AMiniRPGEnemyPawn::StaticClass(), IgnoreActors, OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		AMiniRPGEnemyPawn* Enemy = Cast<AMiniRPGEnemyPawn>(Actor);
		if (!Enemy || !Enemy->StatsComponent)
		{
			continue;
		}

		Enemy->StatsComponent->TakeDamage(StatsComponent->AttackPower);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("Hit!"));
		}

		if (Enemy->StatsComponent->IsDead())
		{
			StatsComponent->GainExperience(Enemy->ExperienceReward);
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("Enemy defeated!"));
			}
			Enemy->Destroy();
		}
		break;
	}
}

void AMiniRPGPlayerPawn::OnUseItemPressed()
{
	if (InventoryComponent && InventoryComponent->Items.Num() > 0)
	{
		InventoryComponent->UseItem(0);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Used an item."));
		}
	}
}

void AMiniRPGPlayerPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GEngine && StatsComponent)
	{
		const FString StatusLine = StatsComponent->IsDead()
			? TEXT("You died. Press Play again to restart.")
			: FString::Printf(TEXT("Lv %d | HP %d/%d | ATK %d | DEF %d | XP %d/%d | Items: %d  (Space=Attack, E=Use Item)"),
				StatsComponent->Level, StatsComponent->CurrentHealth, StatsComponent->MaxHealth,
				StatsComponent->AttackPower, StatsComponent->Defense,
				StatsComponent->Experience, StatsComponent->ExperienceToNextLevel,
				InventoryComponent ? InventoryComponent->Items.Num() : 0);

		GEngine->AddOnScreenDebugMessage(0, 0.0f, FColor::Yellow, StatusLine);
	}
}
