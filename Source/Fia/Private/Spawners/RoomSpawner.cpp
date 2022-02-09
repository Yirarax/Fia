// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawners/RoomSpawner.h"

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

	NormalRooms.Sort(FSortRoomClasses());
	Corridors.Sort(FSortRoomClasses());
	End.Sort(FSortRoomClasses());
	
	if(Rooms.Num() <= 0)
	{
		ARoomBase* First = GetWorld()->SpawnActor<ARoomBase>(NormalRooms.Top());
		Rooms.Add(First);
		First->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		RoomCount++;
		
		FVector Origin, Bounds;
		First->GetActorBounds(true, Origin, Bounds);

		Start = GetWorld()->SpawnActor<APlayerStart>(Origin - (Bounds * 0.3) + FVector{0,0,300},
			FRotator());

		SpawnRoom(First, ERoomType::Room);
	}
}

void ARoomSpawner::SpawnRoom(const ARoomBase* CurrentRoom, const ERoomType CurrentType)
{
	NormalRooms.Sort(FSortRoomClasses());
	Corridors.Sort(FSortRoomClasses());
	End.Sort(FSortRoomClasses());
	
	TArray<TSubclassOf<ARoomBase>> RoomClass = NormalRooms;
	ERoomType ThisType = ERoomType::Room;
	if(CurrentType == ERoomType::Room || (CurrentType == ERoomType::Corridor && FMath::RandRange(0, 100) > 70))
	{
		RoomClass = Corridors;
		ThisType = ERoomType::Corridor;
	}
	
	TArray<UGateway*> Gateways;
	CurrentRoom->GetComponents<UGateway>(Gateways);
		
	for (const auto Gateway : Gateways)
	{
		if(RoomCount >= RoomLimit)
		{
			ThisType = ERoomType::End;
			RoomClass = End;
		}

		FActorSpawnParameters Parameters;
		
		ARoomBase* Spawned = GetWorld()->SpawnActor<ARoomBase>(RoomClass.Top(), Gateway->GetComponentTransform(),
			Parameters);
		
		if(!Spawned)
		{
			for (auto Class : RoomClass)
			{
				Spawned = GetWorld()->SpawnActor<ARoomBase>(Class, Gateway->GetComponentTransform());
				if(Spawned) break;
			}
		}

		if(!Spawned) return;
		RoomCount++;
		Rooms.Add(Spawned);
		Spawned->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		
		if(ThisType != ERoomType::End) SpawnRoom(Spawned, ThisType);
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
	
	if(Start) Start->Destroy();
}
