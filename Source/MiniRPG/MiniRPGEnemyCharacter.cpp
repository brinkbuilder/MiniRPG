#include "MiniRPGEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "RPGStatsComponent.h"
#include "Kismet/GameplayStatics.h"

AMiniRPGEnemyCharacter::AMiniRPGEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(34.0f, 88.0f);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;

	// Same engine-bundled tutorial mesh as the player, tinted red below.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(
		TEXT("/Engine/Tutorial/SubEditors/TutorialAssets/Character/TutorialTPP.TutorialTPP"));
	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBPClass(
		TEXT("/Engine/Tutorial/SubEditors/TutorialAssets/Character/TutorialTPP_AnimBlueprint"));
	if (AnimBPClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);
	}
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	if (UMaterialInstanceDynamic* Dyn = GetMesh()->CreateAndSetMaterialInstanceDynamic(0))
	{
		Dyn->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.75f, 0.1f, 0.1f));
	}

	StatsComponent = CreateDefaultSubobject<URPGStatsComponent>(TEXT("StatsComponent"));
	StatsComponent->MaxHealth = 20;
	StatsComponent->AttackPower = 3;
	StatsComponent->Defense = 0;
}

void AMiniRPGEnemyCharacter::Tick(float DeltaSeconds)
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
