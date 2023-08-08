// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AgentsCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UAgentsAgentWidgetComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AAgentsCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Agents")
	UAgentsAgentWidgetComponent* AgentWidgetComponent;

public:
	AAgentsCharacter();
	
	int32 StepsInEpisode = 0;

protected:
	FVector RespawnLocation;
	float PreviousDistanceFromRespawn = 0.0f;

	FVector PreviousLocation;
	bool bIsOnTarget = false;
	bool bReachedTarget = false;
	bool bHitWall = false;
	bool bInferenceMode = false;
	float CurrentReward = 0.0f;
	float TotalReward = 0.0f;

	float PreviousDistanceToTarget;
	FVector PreviousDirectionOfMovement;

	float LocationTravelledAfterRespawn;

	int32 RespawnId = 1;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void Turn(float Value);

	void Respawn();
	TArray<float> GetObservations() const;
	float GetDistanceFromRespawn() const;

	float GetPreviousDistanceToTarget();
	FVector GetDirectionOfMovement();
	void UpdatePreviousDistanceToTarget(float newDistance);
	void UpdateDirectionOfMovement(FVector newDirection);
	bool IsStuck();
	void AddReward(float Reward);
	void ResetReward();

	float GetPreviousDistanceFromRespawnDelta();
	float GetLocationTravelledAfterRespawn();

	FORCEINLINE FVector GetPreviousDirectionOfMovement() const { return PreviousDirectionOfMovement; }
	FORCEINLINE bool IsOnTarget() const { return bIsOnTarget; }	
	FORCEINLINE bool ReachedTarget() const { return bReachedTarget; }
	FORCEINLINE void SetReachedTarget(bool bReached) { bReachedTarget = bReached; }
	FORCEINLINE bool IsHitWall() const { return bHitWall; }
	FORCEINLINE void SetInferenceMode(bool bInference) { bInferenceMode = bInference; }
	FORCEINLINE void SetPreviousLocation(FVector newLocation) { PreviousLocation = newLocation; }
	FORCEINLINE void SetRespawnId(int32 newId) { RespawnId = newId; }
};

