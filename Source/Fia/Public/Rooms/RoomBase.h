// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Actor.h"
#include "RoomBase.generated.h"

UCLASS()
class FIA_API ARoomBase : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* Facing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 GateNumber;
	
public:
	// Sets default values for this actor's properties
	ARoomBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int32 GetGates() const;
};
