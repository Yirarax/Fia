// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FiaCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AFiaCharacterBase::AFiaCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	CameraComponent->SetupAttachment(SpringArmComponent);

	SpringArmComponent->SetRelativeRotation(FRotator::MakeFromEuler({0, 340, 0}));
	SpringArmComponent->TargetArmLength = 1000.0F;
	
	//GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	//GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void AFiaCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFiaCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AFiaCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AFiaCharacterBase::ChangeZoom(const float Value)
{
	if(SpringArmComponent->TargetArmLength + (Value * 50.0) < 1500.0F
		&& SpringArmComponent->TargetArmLength + (Value * 50.0) > 300.0F)
		SpringArmComponent->TargetArmLength += Value * 50.0F;
}

void AFiaCharacterBase::LookUpCamera(float Value)
{
	SpringArmComponent->AddRelativeRotation(FRotator::MakeFromEuler({0,Value,0}));
}

