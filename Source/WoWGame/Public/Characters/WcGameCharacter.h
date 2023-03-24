#pragma once

#include "GameFramework/Character.h"
#include "WcGameCharacter.generated.h"

class UWcTargetingBehaviorComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class USceneCaptureComponent2D;
class USpringArmComponent;
struct FInputActionValue;

UCLASS(Config=Game)
class WOWGAME_API AWcGameCharacter final : public ACharacter
{
	GENERATED_BODY()

public:

	AWcGameCharacter();

public: // Overriden Functions

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:

	USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void Zoom(const FInputActionValue& Value);

protected: // Set by the Editor

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Input)
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Input)
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Input)
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Input)
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Input)
	TObjectPtr<UInputAction> ZoomAction;

	// By how much will the zoom move the camera when you scroll once.
	UPROPERTY(EditAnywhere, Category=Input)
	float ZoomStep = 50.f;

	// What is the maximum zoom from the character.
	UPROPERTY(EditAnywhere, Category=Input)
	float ZoomMax = 800.f;

	// What is the minimum zoom from the character.
	UPROPERTY(EditAnywhere, Category=Input)
	float ZoomMin = 100.f;

private: // Components

	// Arm positioning the camera that is behind the character.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	// Camera behind character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	// Minimap spring arm
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Minimap, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> MinimapSpringArm;

	// Minimap scene capture
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Minimap, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneCaptureComponent2D> MinimapSceneCapture2D;

	// Targeting Behavior
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Behavior, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWcTargetingBehaviorComponent> TargetingBehavior;
};
