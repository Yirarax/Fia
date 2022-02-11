// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawners/ObjectSpawners.h"

UObjectSpawners::UObjectSpawners()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UObjectSpawners::SpawnObjects(TArray<ARoomBase*> Rooms)
{
	if(HasSpawned())
	{
		Despawn();
	}
	
	for (const auto Room : Rooms)
	{
		for (const auto ObjectClass : ObjectClasses)
		{
			if(Room->GetRoomType() == ERoomType::Corridor && ObjectClass.Value.ObjectType == EObjectType::Treasure) continue;

			const int32 SpawnCount = SpawnLimit(ObjectClass.Value.Density, Room->GetBoundingBoxExtend());

			for(int I = 0; I < SpawnCount; I++)
			{
				const float RandX = Room->GetBoundingBoxExtend().X * 0.5F;
				const float RandY = Room->GetBoundingBoxExtend().Y * 0.5F;
				const FVector Location = FVector{FMath::FRandRange(-RandX, RandX),
					FMath::FRandRange(-RandY, RandY), 0} + Room->GetCenter();
				
				SpawnInternal(ObjectClass.Key, Location, Room->GetBoundingBoxExtend());
				
			}
		}
	}
}

bool UObjectSpawners::SpawnInternal(const TSubclassOf<AActor> SpawnClass, FVector Location, const FVector Bounds)
{
	UClass* ActorClass = SpawnClass.Get();

	//FHitResult Result;
	//GetWorld()->LineTraceSingleByChannel(Result, {Location.X, Location.Y, 9999.9F},
	//	{Location.X, Location.Y, -9999.9F},ECC_WorldStatic);
	
	TArray<FHitResult> Hits;
	GetWorld()->SweepMultiByChannel(Hits, Location,Location,FQuat(), ECC_WorldStatic,
		FCollisionShape::MakeBox(Bounds * 0.5));

	for (auto Hit : Hits)
	{
		if(Hit.GetActor()->IsA(UStaticMeshComponent::StaticClass()))
		{
			Location = Hit.ImpactPoint;
			AActor* SpawnedActor = GetWorld()->SpawnActor(ActorClass, &Location);
			if(SpawnedActor)
			{	
				SpawnedActors.Add(SpawnedActor);
				SpawnedActor->AttachToActor(GetOwner(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
				return true;
			}
		}
	}
	return false;
}

bool UObjectSpawners::HasSpawned() const
{
	return SpawnedActors.Num() > 0;
}

int32 UObjectSpawners::SpawnLimit(const float SpawnDensity, const FVector Bounds)
{
	return FMath::RoundToInt(SpawnDensity * 0.01 * Bounds.Size());
}

void UObjectSpawners::Despawn()
{
	for (const auto Result : SpawnedActors)
	{
		if(Result) Result->Destroy();
	}
	SpawnedActors.Empty();
}
