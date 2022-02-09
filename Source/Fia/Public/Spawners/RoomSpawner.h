// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerStart.h"
#include "Rooms/RoomBase.h"
#include "RoomSpawner.generated.h"

UENUM(BlueprintType)
enum class ERoomType : uint8
{
	Room,
	Corridor,
	End
};

struct FSortRoomClasses
{
	FORCEINLINE bool operator()(const TSubclassOf<ARoomBase>& A, const TSubclassOf<ARoomBase>& B) const
	{
		return FMath::RandBool();
	}
};

UCLASS()
class FIA_API ARoomSpawner : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere);
	TMap<TSubclassOf<ARoomBase>, ERoomType> RoomClasses;
	
	UPROPERTY()
	TArray<ARoomBase*> Rooms;

	UPROPERTY()
	APlayerStart* Start;

	UPROPERTY(BlueprintReadWrite, EditAnywhere);
	int32 RoomLimit;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere);
	int32 RoomCount;
public:
	// Sets default values for this actor's properties
	ARoomSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(CallInEditor)
	void GenerateRooms();

	void SpawnRoom(const ARoomBase* CurrentRoom, int32 CorridorCount);
	
	UFUNCTION(CallInEditor)
	void CleanRooms();
	static FString TypeToString(ERoomType Type);
};
