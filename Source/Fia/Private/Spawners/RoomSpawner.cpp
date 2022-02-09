// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawners/RoomSpawner.h"

#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Fia/Fia.h"
#include "GameFramework/PlayerStart.h"
#include "Misc/Gateway.h"


// Sets default values
ARoomSpawner::ARoomSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RoomLimit = 15;
	RoomCount = 0;
}

// Called when the game starts or when spawned
void ARoomSpawner::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARoomSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARoomSpawner::GenerateRooms()
{
	CleanRooms();

	for (const auto RoomClass : RoomClasses)
	{
		if (RoomClass.Value == ERoomType::Room)
		{
			ARoomBase* First = GetWorld()->SpawnActor<ARoomBase>(RoomClass.Key);
			Rooms.Add(First);
			First->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			RoomCount++;

			FVector Origin, Bounds;
			First->GetActorBounds(true, Origin, Bounds);

			Start = GetWorld()->SpawnActor<APlayerStart>(Origin - (Bounds * 0.3) + FVector{0, 0, 300},
			                                             FRotator());

			UE_LOG(LogFiaRoomSpawners, Display, TEXT("Spawner: ""%s"" - First Room Spawned: ""%s"" - Type: ""%s"),
				*GetName(), *First->GetName(), *TypeToString(ERoomType::Room));
			SpawnRoom(First, 0);
			break;
		}
	}
}

void ARoomSpawner::SpawnRoom(const ARoomBase* CurrentRoom, int32 CorridorCount)
{
	RoomClasses.KeySort(FSortRoomClasses());

	TArray<UGateway*> Gateways;
	CurrentRoom->GetComponents<UGateway>(Gateways);
	
	for (const auto Gateway : Gateways)
	{
		bool bSpawnedAny = false;
		// find out which way is right
		const FRotator Rotation = Gateway->GetComponentRotation();
		
		for (auto RoomClass : RoomClasses)
		{
			const ARoomBase* RoomBase = Cast<ARoomBase>(RoomClass.Key.Get()->ClassDefaultObject);
			
			FVector Bounds = Rotation.RotateVector(RoomBase->GetBoundingBoxExtend());
			FVector Origin = Gateway->GetComponentTransform().GetLocation() + Rotation.RotateVector(RoomBase->GetCenter());
			
			FCollisionObjectQueryParams QueryParams;
			QueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel2);
			TArray<FHitResult> Hits;
			
			//FColorList List;
			//List.CreateColorMap();
			//FColor LocalColour = List.GetFColorByIndex(FMath::RandRange(0, List.GetColorsNum()));
			
		//UE_LOG(LogFiaRoomSpawners, Display, TEXT("Spawner: ""%s"" - Evaluating: ""%s"", Bounds %s"),
		//	*GetName(), *(RoomClass.Key->GetDefaultObject<ARoomBase>()->GetName()), *Bounds.ToString());
			
			if (!GetWorld()->SweepMultiByObjectType(Hits, Origin, Origin, FQuat(), QueryParams,
			                                        FCollisionShape::MakeBox(Bounds)))
			{
				if ((RoomClass.Value == ERoomType::End && RoomCount >= RoomLimit) ||
					(!(RoomClass.Value == RoomClasses.FindRef(CurrentRoom->GetClass()) && RoomClass.Value ==
					ERoomType::Room) && RoomCount < RoomLimit && !(RoomClass.Value == ERoomType::Corridor && CorridorCount > 3)
					&& RoomClass.Value != ERoomType::End))
				{
					ARoomBase* Spawned = GetWorld()->SpawnActor<ARoomBase>(
						RoomClass.Key, Gateway->GetComponentTransform());

					if(RoomClass.Value == ERoomType::Corridor) CorridorCount++;
					else CorridorCount = 0;
					
					if (!Spawned)
					{
						UE_LOG(LogFiaRoomSpawners, Error, TEXT("Spawner: ""%s"" - Tried Spawning Room: ""%s"" - Error!"),
							*GetName(), *(RoomClass.Key->GetDefaultObject<ARoomBase>()->GetName()));
						return;
					}
					//DrawDebugBox(GetWorld(), Origin, Bounds, LocalColour, false, 180, 2, 15);
					//DrawDebugLine(GetWorld(), Origin * FVector(1,1,-1000), Origin * FVector(1,1,1000), LocalColour, false, 180, 3, 50);
			
					RoomCount++;
					Rooms.Add(Spawned);
					Spawned->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
					UE_LOG(LogFiaRoomSpawners, Display, TEXT("Spawner: ""%s"" - Room Spawned: ""%s"" - Type: ""%s"" - %d / %d"),
					*GetName(), *Spawned->GetName(), *TypeToString(RoomClass.Value) ,RoomCount, RoomLimit);
					bSpawnedAny = true;
					SpawnRoom(Spawned, CorridorCount);
					break;
				}
			}
			else
			{
				for (auto Hit : Hits)
				{
					UE_LOG(LogFiaRoomSpawners, Warning, TEXT("Spawner: ""%s"" - Hit Found at %s -  Error!"),
					*GetName(), *(Hit.Location.ToString()));
				}
			}
		}

		if (!bSpawnedAny)
		{
			UE_LOG(LogFiaRoomSpawners, Error, TEXT("Spawner: ""%s"" - Could not spawn any Room -  Error!"),
				*GetName());
			return;
		}
	}
}

void ARoomSpawner::CleanRooms()
{
	for (const auto Room : Rooms)
	{
		Room->Destroy();
	}
	Rooms.Empty();
	RoomCount = 0;

	if (Start) Start->Destroy();
}

FString ARoomSpawner::TypeToString(const ERoomType Type)
{
	switch (Type) {
	case ERoomType::Room: return "Room";
	case ERoomType::Corridor: return "Corridor";
	case ERoomType::End: return "End";
	default: ;
	}
	return "None";
}
