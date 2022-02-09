// Fill out your copyright notice in the Description page of Project Settings.


#include "FiaPlayerController.h"


// Sets default values
AFiaPlayerController::AFiaPlayerController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AFiaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	InputComponent->BindAxis("MoveForward", this, &AFiaPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AFiaPlayerController::MoveRight);
	InputComponent->BindAxis("MouseHorizontal", this, &AFiaPlayerController::TurnRight);
	InputComponent->BindAxis("MouseVertical", this, &AFiaPlayerController::TurnUp);

	if(!FiaCharacter) return;
	InputComponent->BindAxis("Zoom", FiaCharacter, &AFiaCharacterBase::ChangeZoom);
}

void AFiaPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	FiaCharacter = Cast<AFiaCharacterBase>(InPawn);
}

// Called every frame
void AFiaPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFiaPlayerController::MoveForward(const float In)
{
	if (In != 0.0f)
	{
		// find out which way is forward
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		GetPawn()->AddMovementInput(Direction, In);
	}
}

void AFiaPlayerController::MoveRight(const float In)
{
	if (In != 0.0f)
	{
		// find out which way is right
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		GetPawn()->AddMovementInput(Direction, In);
	}
}

void AFiaPlayerController::TurnUp(const float In)
{
	if(FiaCharacter) FiaCharacter->LookUpCamera(In);
}

void AFiaPlayerController::TurnRight(const float In)
{
	AddYawInput(In);
}


