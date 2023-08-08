// Copyright Epic Games, Inc. All Rights Reserved.

#include "AgentsCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "AgentsTarget.h"
#include "AgentsSpawn.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Agents/HUD/AgentsAgentWidgetComponent.h"
#include "Components/WidgetComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AAgentsCharacter

AAgentsCharacter::AAgentsCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true; 


	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; 

	AgentWidgetComponent = CreateDefaultSubobject<UAgentsAgentWidgetComponent>(TEXT("AgentWidget"));
	AgentWidgetComponent->SetupAttachment(RootComponent);

}

void AAgentsCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AAgentsCharacter::OnOverlapBegin);
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AAgentsCharacter::OnComponentHit);
}

void AAgentsCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bHitWall = false;

	AAgentsTarget* Target = nullptr;
	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		for (TActorIterator<AAgentsTarget> It(World); It; ++It)
		{
			Target = *It;

			break;
		}
	}

	if (FVector::Dist(GetActorLocation(), Target->GetActorLocation()) < 250.0f)
	{
		bIsOnTarget = true;
	}
	else
	{
		bIsOnTarget = false;
	}
}

void AAgentsCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{

		AAgentsTarget* Target = Cast<AAgentsTarget>(OtherActor);


		if (Target && bInferenceMode)
		{
			if (ExplosionEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, GetActorLocation(), GetActorRotation());
			}

			Respawn();
		}
	}
}

void AAgentsCharacter::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherComp && OtherComp->IsA(UStaticMeshComponent::StaticClass()))
	{
		bHitWall = true;
	}
	else
	{
		bHitWall = false;
	}
}

void AAgentsCharacter::Respawn()
{
	bIsOnTarget = false;

	AAgentsTarget* Target = nullptr;
	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		for (TActorIterator<AAgentsTarget> It(World); It; ++It)
		{
			Target = *It;

			break;
		}
	}

	if (World != nullptr)
	{
		AAgentsSpawn* AgentsSpawn = nullptr;

		if (bInferenceMode)
		{
			FString Tag = FString::Printf(TEXT("InferenceSpawn"));
			TArray<AActor*> FoundActors;

			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(*Tag), FoundActors);

			if (FoundActors.Num() > 0)
			{
				AgentsSpawn = Cast<AAgentsSpawn>(FoundActors[0]);
			}
		}
		else
		{
			FString Tag = FString::Printf(TEXT("Respawn_%d"), RespawnId);
			TArray<AActor*> FoundActors;
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(*Tag), FoundActors);

			if (FoundActors.Num() > 0)
			{
				AgentsSpawn = Cast<AAgentsSpawn>(FoundActors[0]);
			}
		}

		if (AgentsSpawn)
		{
			do {
				FVector RespawnLocation = AgentsSpawn->GetSpawnLocation();
				RespawnLocation.Z = 700.0f;
				

				float RandomZRotation = FMath::RandRange(0.f, 360.f);
				FRotator SpawnRotation(0, RandomZRotation, 0);

				FVector Start = RespawnLocation;
				FVector End = Start + FVector(0, 0, -1000);
				FHitResult HitResult;
				FCollisionQueryParams CollisionParams;
				CollisionParams.AddIgnoredActor(this);

				if (World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
				{
					RespawnLocation = HitResult.Location;
					RespawnLocation.Z += 91.0f;
				}

				SetActorLocation(RespawnLocation);
				SetActorRotation(SpawnRotation);

			} while (FVector::Dist(GetActorLocation(), Target->GetActorLocation()) < 450.0f);	
		}
	}
}

TArray<float> AAgentsCharacter::GetObservations() const
{
	TArray<float> Observations;

	FVector ActorLocation = GetActorLocation();
	FRotator ActorRotation = GetActorRotation();

	Observations.Add(ActorLocation.X);
	Observations.Add(ActorLocation.Y);
	Observations.Add(ActorRotation.Yaw);
		
	return Observations;
}


void AAgentsCharacter::AddReward(float Reward)
{
	CurrentReward = Reward;
	TotalReward += Reward;

	FString RewardText = FString::Printf(TEXT("Current: %.2f, Total: %.2f"), CurrentReward, TotalReward);
	AgentWidgetComponent->SetText(RewardText);
}

void AAgentsCharacter::ResetReward()
{
	CurrentReward = 0.0f;
	TotalReward = 0.0f;

	FString RewardText = FString::Printf(TEXT("Current: %.2f, Total: %.2f"), CurrentReward, TotalReward);
	AgentWidgetComponent->SetText(RewardText);
}

//////////////////////////////////////////////////////////////////////////
// Input


void AAgentsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAgentsCharacter::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAgentsCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AAgentsCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y < 0 ? MovementVector.Y / 5 : MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AAgentsCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AAgentsCharacter::Turn(float Value)
{
	FRotator CurrentRotation = GetActorRotation(); 
	CurrentRotation.Yaw += Value;
	SetActorRotation(CurrentRotation);
}
