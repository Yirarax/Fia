// Fill out your copyright notice in the Description page of Project Settings.


#include "Rooms/RoomBase.h"

#include "Misc/Gateway.h"


// Sets default values
ARoomBase::ARoomBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Facing = CreateDefaultSubobject<UArrowComponent>(TEXT("Facing"));
	SetRootComponent(Facing);

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
	Facing->SetMobility(EComponentMobility::Static);
	
	GateNumber = 0;
}

// Called when the game starts or when spawned
void ARoomBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARoomBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

int32 ARoomBase::GetGates() const
{
	TArray<UGateway*> Gateways;
	GetComponents<UGateway>(Gateways);
	return Gateways.Num();
}