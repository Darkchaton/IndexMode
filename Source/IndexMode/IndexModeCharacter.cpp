// Copyright Epic Games, Inc. All Rights Reserved.

#include "IndexModeCharacter.h"
#include "IndexModeProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h" 
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "PlayerWidget.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AIndexModeCharacter

AIndexModeCharacter::AIndexModeCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

}

void AIndexModeCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//https://stackoverflow.com/questions/61024803/how-to-add-widget-in-unreal-engine
	auto UserWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerWidgetClass);
	PlayerWidget = Cast<UPlayerWidget>(UserWidget);
	PlayerWidget->AddToViewport();
}

//////////////////////////////////////////////////////////////////////////// Input

void AIndexModeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AIndexModeCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AIndexModeCharacter::Look);

		//Enter
		EnhancedInputComponent->BindAction(EnterInspectAction, ETriggerEvent::Triggered, this, &AIndexModeCharacter::EnterInspect);

		//exit
		EnhancedInputComponent->BindAction(ExitInspectAction, ETriggerEvent::Triggered, this, &AIndexModeCharacter::ExitInspect);

		//rotate
		EnhancedInputComponent->BindAction(RotateInspectAction, ETriggerEvent::Triggered, this, &AIndexModeCharacter::RotateInspect);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AIndexModeCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AIndexModeCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AIndexModeCharacter::EnterInspect()
{
	auto PlayerController = Cast<APlayerController>(GetController());
	auto InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	InputSubsystem->RemoveMappingContext(DefaultMappingContext);
	InputSubsystem->AddMappingContext(InspectMappingContext, 0);
}

void AIndexModeCharacter::ExitInspect()
{
	auto PlayerController = Cast<APlayerController>(GetController());
	auto InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	InputSubsystem->RemoveMappingContext(InspectMappingContext);
	InputSubsystem->AddMappingContext(DefaultMappingContext, 0);
}

void AIndexModeCharacter::RotateInspect(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("RotateInspect"));
}
