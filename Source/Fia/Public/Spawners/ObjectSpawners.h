// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Rooms/RoomBase.h"
#include "ObjectSpawners.generated.h"

UENUM(BlueprintType)
enum class EObjectType : uint8
{
	Enemy,
	Trap,
	Treasure,
	Environment
};

USTRUCT(BlueprintType)
struct FObjectInformation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EObjectType ObjectType;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float Density;

	FObjectInformation(): ObjectType(EObjectType::Environment), Density(1.0F)
	{
	}
};

/**
 * 
 */
UCLASS(BlueprintType, DefaultToInstanced)
class FIA_API UObjectSpawners : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TSubclassOf<AActor>, FObjectInformation> ObjectClasses;
	UPROPERTY()
	TArray<AActor*> SpawnedActors;
	
public:
	UObjectSpawners();

	void SpawnObjects(TArray<ARoomBase*> Rooms);
	bool SpawnInternal(TSubclassOf<AActor> SpawnClass, FVector Location, FVector Bounds);
	bool HasSpawned() const;
	static int32 SpawnLimit(float SpawnDensity, const FVector Bounds);
	void Despawn();
};
