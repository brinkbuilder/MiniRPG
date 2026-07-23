#include "InventoryComponent.h"
#include "RPGStatsComponent.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::AddItem(const FInventoryItem& Item)
{
	Items.Add(Item);
}

void UInventoryComponent::UseItem(int32 Index)
{
	if (!Items.IsValidIndex(Index))
	{
		return;
	}

	if (URPGStatsComponent* Stats = GetOwner()->FindComponentByClass<URPGStatsComponent>())
	{
		const FInventoryItem& Item = Items[Index];
		switch (Item.Type)
		{
		case EItemType::Potion:
			Stats->Heal(Item.Value);
			break;
		case EItemType::WeaponUpgrade:
			Stats->AttackPower += Item.Value;
			break;
		}
	}

	Items.RemoveAt(Index);
}
