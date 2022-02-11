// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/Gateway.h"


// Sets default values for this component's properties
UGateway::UGateway()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	bHiddenInGame = true;
}


// Called when the game starts
void UGateway::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGateway::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UGateway::IsFree() const
{
	return NextActor == nullptr;
}

void UGateway::UseGateway(AActor* Actor)
{
	NextActor = Actor;
}

void UGateway::FreeGateway()
{
	NextActor = nullptr;
}