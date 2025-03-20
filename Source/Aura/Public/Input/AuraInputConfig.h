// Copyright Kent27727

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AuraInputConfig.generated.h"

USTRUCT(BlueprintType)
struct FAuraInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	const class UInputAction* InputAction = nullptr;

};

UCLASS()
class AURA_API UAuraInputConfig : public UDataAsset
{
	GENERATED_BODY()


public:
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<FAuraInputAction> AbilityInputActions;
};
