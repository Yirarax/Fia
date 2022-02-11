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

	ObjectSpawner = CreateDefaultSubobject<UObjectSpawners>(TEXT("Object Spawner"));
	
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

void ARoomSpawner::GenerateObjects() const
{
	if(ObjectSpawner) ObjectSpawner->SpawnObjects(Rooms);
}

void ARoomSpawner::GenerateRooms()
{
	CleanRooms();

	for (const auto RoomClass : RoomClasses)
	{
		if (ARoomBase::GetStaticRoomType(RoomClass) == ERoomType::Room)
		{
			ARoomBase* First = GetWorld()->SpawnActor<ARoomBase>(RoomClass);
			Rooms.Add(First);
			First->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			RoomCount++;
			
			FHitResult Hit;
			if(GetWorld()->LineTraceSingleByChannel(Hit, First->GetCenter() + FVector(0,0,2000),
				First->GetCenter() - FVector(0,0,2000), ECC_WorldStatic))
			{
				Start = GetWorld()->SpawnActor<APlayerStart>(Hit.ImpactPoint + FVector{0, 0, 150},
															 FRotator());
			}
			
			UE_LOG(LogFiaRoomSpawners, Display, TEXT("Spawner: ""%s"" - First Room Spawned: ""%s"" - Type: ""%s"),
				*GetName(), *First->GetName(), *TypeToString(ERoomType::Room));
			
			SpawnRoom(First, 0);
			break;
		}
	}

	TArray<TSubclassOf<ARoomBase>> Ends;
	for (auto Class : RoomClasses)
	{
		if(ARoomBase::GetStaticRoomType(Class) == ERoomType::End) Ends.Add(Class);
	}
	
	for (const auto Room : Rooms)
	{
		TArray<UGateway*> Gateways;
		Room->GetComponents<UGateway>(Gateways);

		if(Gateways.Num() <= 0) continue;
		for (const auto Gateway : Gateways)
		{
			if(Gateway->IsFree()) SpawnRoom(Room, 0);
		}
	}
}

void ARoomSpawner::SpawnRoom(const ARoomBase* CurrentRoom, int32 CorridorCount)
{
	RoomClasses.Sort(FSortRoomClasses());

	TArray<UGateway*> Gateways;
	CurrentRoom->GetComponents<UGateway>(Gateways);

	TArray<ARoomBase*> LastSpawned;
	
	for (auto Gateway : Gateways)
	{
		bool bSpawnedAny = false;
		const FRotator Rot = Gateway->GetComponentRotation();

		if(!Gateway->IsFree()) continue;
		
		for (auto RoomClass : RoomClasses)
		{
			const ARoomBase* RoomBase = Cast<ARoomBase>(RoomClass.Get()->ClassDefaultObject);
			
			FVector Bounds = Rot.RotateVector(RoomBase->GetBoundingBoxExtend());
			FVector Origin = Gateway->GetComponentTransform().GetLocation() + Rot.RotateVector(RoomBase->GetCenter());
			
			FCollisionObjectQueryParams QueryParams;
			QueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel2);
			TArray<FHitResult> Hits;
			
			if (!GetWorld()->SweepMultiByObjectType(Hits, Origin, Origin, FQuat(), QueryParams,
			                                        FCollisionShape::MakeBox(Bounds * 0.98)))
			{
				if(RoomCount >= RoomLimit && ARoomBase::GetStaticRoomType(RoomClass) != ERoomType::End) continue;
				if(ARoomBase::GetStaticRoomType(RoomClass) == CurrentRoom->GetRoomType() && ARoomBase::GetStaticRoomType(RoomClass) ==
					ERoomType::Room) continue;
				if(ARoomBase::GetStaticRoomType(RoomClass) == ERoomType::Corridor && CorridorCount > 3)
				{
					UE_LOG(LogFiaRoomSpawners, Warning, TEXT("Spawner: ""%s"" - Path Limit Reached!"),
							*GetName());
					continue;
				}
				if(RoomCount < RoomLimit &&  ARoomBase::GetStaticRoomType(RoomClass) == ERoomType::End) continue;
				
				ARoomBase* Spawned = GetWorld()->SpawnActor<ARoomBase>(RoomClass, Gateway->GetComponentTransform());
				Gateway->UseGateway(Spawned);
				if(ARoomBase::GetStaticRoomType(RoomClass) == ERoomType::Corridor) CorridorCount++;
				else CorridorCount = 0;
				
				if (!Spawned)
				{
					UE_LOG(LogFiaRoomSpawners, Error, TEXT("Spawner: ""%s"" - Tried Spawning Room: ""%s"" - Error!"),
						*GetName(), *(RoomClass->GetDefaultObject<ARoomBase>()->GetName()));
					return;
				}
				//DrawDebugBox(GetWorld(), Origin, Bounds, LocalColour, false, 180, 2, 15);
				//DrawDebugLine(GetWorld(), Origin * FVector(1,1,-1000), Origin * FVector(1,1,1000), LocalColour, false, 180, 3, 50);
		
				RoomCount++;
				Rooms.Add(Spawned);
				Spawned->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
				UE_LOG(LogFiaRoomSpawners, Display, TEXT("Spawner: ""%s"" - Room Spawned: ""%s"" - Type: ""%s"" - %d / %d"),
				*GetName(), *Spawned->GetName(), *TypeToString(ARoomBase::GetStaticRoomType(RoomClass)) ,RoomCount, RoomLimit);
				bSpawnedAny = true;
				LastSpawned.Add(Spawned);
				break;
			}
			for (auto Hit : Hits)
			{
				UE_LOG(LogFiaRoomSpawners, Warning, TEXT("Spawner: ""%s"" - Hit Found at %s -  Error!"),
				*GetName(), *(Hit.Location.ToString()));
			}
		}

		if (!bSpawnedAny)
		{
			UE_LOG(LogFiaRoomSpawners, Error, TEXT("Spawner: ""%s"" - Could not spawn any Room -  Error!"),
				*GetName());
		}
	}

	for (auto Spawned : LastSpawned)
	{
		SpawnRoom(Spawned, CorridorCount);
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
