// Copyright Kent27727

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

/**
 * 
 */
class UInputMappingContext;
class UInputAction;
class IEnemyInterface;
class UAuraInputConfig;
class USplineComponent;

struct FInputActionValue;
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
	

public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	void Move(const FInputActionValue& InputActionValue);

	void CursorTrace();
	void AutoRun();
	FHitResult CursorHit;
	
	UFUNCTION()
	void AbilityInputTagPressed();

	UFUNCTION()
	void AbilityInputTagReleased();

	UFUNCTION()
	void AbilityInputTagHeld();

	FVector CachedDestination = FVector::ZeroVector;
	bool bAutoRunning = false;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	TScriptInterface<IEnemyInterface> LastActor;
	TScriptInterface<IEnemyInterface> ThisActor;
};
