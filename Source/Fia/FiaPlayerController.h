// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/FiaCharacterBase.h"
#include "FiaPlayerController.generated.h"

UCLASS()
class FIA_API AFiaPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	AFiaCharacterBase* FiaCharacter;
	
public:
	// Sets default values for this actor's properties
	AFiaPlayerController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void MoveForward(const float In);
	void MoveRight(const float In);

	void TurnUp(const float In);
	void TurnRight(const float In);
};
