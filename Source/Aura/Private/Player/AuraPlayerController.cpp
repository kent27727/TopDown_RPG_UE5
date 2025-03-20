// Copyright Kent27727


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Interaction/EnemyInterface.h"
#include "Components/SplineComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

#include "Input/AuraInputComponent.h"


AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	AutoRun();
}


void AAuraPlayerController::AutoRun() 
{

	if (!bAutoRunning) return;

	if (APawn* ControlledPawn=GetPawn())
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
			ControlledPawn->AddMovementInput(Direction);
		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination<=100.f)
		{
			bAutoRunning = false;
		}
	}


	/*if (APawn* ControlledPawn=GetPawn())
	{

		const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection);
	}*/
}

void AAuraPlayerController::CursorTrace()
{
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	const ECollisionChannel TraceChannel = ECC_Visibility;
	GetHitResultUnderCursor(TraceChannel, false, CursorHit);
	if (!CursorHit.bBlockingHit)
		return;

	LastActor = ThisActor;
	ThisActor = CursorHit.GetActor();

	/**
	* A. LastActor is null && ThisActor is null.
	*	-Nothing
	* B. LastActor is null && ThisActor is valid.
	*	-Highlight ThisActor
	* C. LastActor is valid && ThisActor is null.
	*	-UnHighlight LastActor
	* D. Both actors are valid, but LastActor != ThisActor.
	*	-UnHighlight LastActor, and HighLight This Actor
	* E. Both actors are valid, and are the same actor.
	*	-Nothing
	*/

	if (LastActor==nullptr)
	{
		if (ThisActor !=nullptr)
		{
			//Case B
			ThisActor->HighlightActor();
		}
		else
		{
			//Case A
		}

	}
	else
	{
		if (ThisActor==nullptr)
		{
			//Case C
			LastActor->UnHighlightActor();
		}
		else
		{
			if (LastActor!=ThisActor)
			{
				//Case D
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}
			else
			{
				//Case E
				
			}
			
		}

	}

}



void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);



	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);
	Subsystem->AddMappingContext(AuraContext, 0);
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* AuraInputComponent = CastChecked< UAuraInputComponent>(InputComponent);

	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);

}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection= FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn=GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection,InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::AbilityInputTagPressed()
{
	CachedDestination = CursorHit.ImpactPoint;
	bAutoRunning = false;

}

void AAuraPlayerController::AbilityInputTagReleased()
{

	if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, GetPawn()->GetActorLocation(), CachedDestination))
	{
		Spline->ClearSplinePoints();
		for (const FVector& PointLoc : NavPath->PathPoints)
		{
			Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
		}
		if (NavPath->PathPoints.Num() > 0)
		{
			CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
			bAutoRunning = true;
				
		}
		
	}
}
	


void AAuraPlayerController::AbilityInputTagHeld()
{
	
	if (CursorHit.bBlockingHit)
	{
		CachedDestination = CursorHit.ImpactPoint;
	}
}


