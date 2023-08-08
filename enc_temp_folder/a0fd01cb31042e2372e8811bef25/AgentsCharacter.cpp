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
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	AgentWidgetComponent = CreateDefaultSubobject<UAgentsAgentWidgetComponent>(TEXT("AgentWidget"));
	AgentWidgetComponent->SetupAttachment(RootComponent);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AAgentsCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
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

	if (FVector::Dist(GetActorLocation(), Target->GetActorLocation()) < 50.0f)
	{
		bIsOnTarget = true;
	}
	else
	{
		bIsOnTarget = false;
	}

	float CurrentDistanceToTarget = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
	UpdatePreviousDistanceToTarget(CurrentDistanceToTarget);

	FVector CurrentDirectionOfMovement = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	UpdateDirectionOfMovement(CurrentDirectionOfMovement);
}

void AAgentsCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{

		// Check if the actor is of type AAgentsTarget
		AAgentsTarget* Target = Cast<AAgentsTarget>(OtherActor);

		// bIsOnTarget = Target != nullptr;

		if (Target && bInferenceMode)
		{
			Respawn();
		}
	}
}

void AAgentsCharacter::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherComp && OtherComp->IsA(UStaticMeshComponent::StaticClass()))
	{
		bHitWall = true;
		// UE_LOG(LogTemplateCharacter, Warning, TEXT("Hit Wall"));
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
			/*FString Tag = FString::Printf(TEXT("TestSpawn"));
			TArray<AActor*> FoundActors;

			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(*Tag), FoundActors);

			if (FoundActors.Num() > 0)
			{
				AgentsSpawn = Cast<AAgentsSpawn>(FoundActors[0]);
			}*/

			/*TArray<AAgentsSpawn*> SpawnPoints;
			for (TActorIterator<AAgentsSpawn> It(World); It; ++It)
			{
				SpawnPoints.Add(*It);
			}

			if (SpawnPoints.Num() > 0) 
			{
				int32 RandomIndex = FMath::RandRange(0, SpawnPoints.Num() - 1); 
				AgentsSpawn = SpawnPoints[RandomIndex]; 
			}*/

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
				RespawnLocation = AgentsSpawn->GetSpawnLocation();
				RespawnLocation.Z = 700.0f; // You might need to adjust this height based on your specific setup
				PreviousLocation = RespawnLocation;
				

				float RandomZRotation = FMath::RandRange(0.f, 360.f);
				FRotator SpawnRotation(0, RandomZRotation, 0);

				// Perform a line trace to find the ground
				FVector Start = RespawnLocation;
				FVector End = Start + FVector(0, 0, -1000); // 1000 is the trace distance, adjust as needed
				FHitResult HitResult;
				FCollisionQueryParams CollisionParams;
				CollisionParams.AddIgnoredActor(this);

				if (World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
				{
					// If the trace hits something, set the respawn location to the hit location
					RespawnLocation = HitResult.Location;
					RespawnLocation.Z += 91.0f;
				}

				SetActorLocation(RespawnLocation);
				SetActorRotation(SpawnRotation);

			} while (FVector::Dist(GetActorLocation(), Target->GetActorLocation()) < 450.0f);	

			LocationTravelledAfterRespawn = 0.0f;
		}
	}
}

TArray<float> AAgentsCharacter::GetObservations() const
{
	TArray<float> Observations;

	FVector ActorLocation = GetActorLocation();
	FRotator ActorRotation = GetActorRotation();

	/*float LocationBracketSize = 5.0f;
	float RotationBracketSize = 5.0f;

	Observations.Add(FMath::RoundToInt(ActorLocation.X / LocationBracketSize) * LocationBracketSize);
	Observations.Add(FMath::RoundToInt(ActorLocation.Y / LocationBracketSize) * LocationBracketSize);
	Observations.Add(FMath::RoundToInt(ActorLocation.Z / LocationBracketSize) * LocationBracketSize);
	Observations.Add(FMath::RoundToInt(ActorRotation.Pitch / RotationBracketSize) * RotationBracketSize);
	Observations.Add(FMath::RoundToInt(ActorRotation.Yaw / RotationBracketSize) * RotationBracketSize);
	Observations.Add(FMath::RoundToInt(ActorRotation.Roll / RotationBracketSize) * RotationBracketSize);*/

	//UE_LOG(LogTemp, Warning, TEXT("Observations: %f %f %f, %f %f %f"), 
	//	Observations[0], Observations[1], Observations[2],
	//	Observations[3], Observations[4], Observations[5]);

	//Observations.Add(RespawnLocation.X);
	//Observations.Add(RespawnLocation.Y);
	//Observations.Add(RespawnLocation.Z);
	Observations.Add(ActorLocation.X);
	Observations.Add(ActorLocation.Y);
	//Observations.Add(ActorLocation.Z);
	//Observations.Add(ActorRotation.Pitch);
	Observations.Add(ActorRotation.Yaw);
	//Observations.Add(ActorRotation.Roll);

	// UE_LOG(LogTemp, Warning, TEXT("Observations: %f %f %f, %f"), Observations[0], Observations[1], Observations[2], Observations[3]);
	
	return Observations;
}

float AAgentsCharacter::GetDistanceFromRespawn() const
{
	FVector ActorLocation = GetActorLocation();

	return FVector::Dist(ActorLocation, RespawnLocation);
}

float AAgentsCharacter::GetPreviousDistanceToTarget()
{
	return PreviousDistanceToTarget;
}

FVector AAgentsCharacter::GetDirectionOfMovement()
{
	return PreviousDirectionOfMovement;
}

void AAgentsCharacter::UpdatePreviousDistanceToTarget(float newDistance)
{
	PreviousDistanceToTarget = newDistance;
}

void AAgentsCharacter::UpdateDirectionOfMovement(FVector newDirection)
{
	PreviousDirectionOfMovement = newDirection;
}

bool AAgentsCharacter::IsStuck()
{
	return FVector::Dist(PreviousLocation, GetActorLocation()) < 1.0f;
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

float AAgentsCharacter::GetPreviousDistanceFromRespawnDelta()
{
	float Result = GetDistanceFromRespawn() - PreviousDistanceFromRespawn;
	PreviousDistanceFromRespawn = GetDistanceFromRespawn();

	return Result;
}

float AAgentsCharacter::GetLocationTravelledAfterRespawn()
{
	float Sum = FVector::Dist(PreviousLocation, GetActorLocation());
	PreviousLocation = GetActorLocation();

	LocationTravelledAfterRespawn += Sum;

	return LocationTravelledAfterRespawn;
}

//////////////////////////////////////////////////////////////////////////
// Input


void AAgentsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAgentsCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAgentsCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AAgentsCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// UE_LOG(LogTemplateCharacter, Warning, TEXT("Move: %f %f"), MovementVector.X, MovementVector.Y);

		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y < 0 ? MovementVector.Y / 5 : MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AAgentsCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Look: %f %f"), LookAxisVector.X, LookAxisVector.Y);

		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AAgentsCharacter::Turn(float Value)
{
	// UE_LOG(LogTemp, Warning, TEXT("Turn: %f"), Value);

	FRotator CurrentRotation = GetActorRotation(); 
	CurrentRotation.Yaw += Value;
	SetActorRotation(CurrentRotation);
}
