// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "ShootAndRun/SarComponents/CombatComponent.h"
#include "ShootAndRun/SarTypes/TurningInPlace.h"
#include "ShootAndRun/Interfaces/InteractWithCrosshairsInterface.h"
#include "Sound/SoundCue.h"
#include "ShootAndRun/SarTypes/CombatState.h"
#include "SarCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGame);

UCLASS()
class SHOOTANDRUN_API ASarCharacter : public ACharacter, public IInteractWithCrosshairsInterface 
{
	GENERATED_BODY()

public:
	ASarCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayElimMontage();
	virtual void OnRep_ReplicatedMovement() override;
	void Elim(bool bPlayerLeftGame);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim(bool bPlayerLeftGame);
	virtual void Destroyed() override;

	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	void UpdateHUDHealth();
	void UpdateHUDAmmo();

	/*
	 *	Hit boxed used for server-side rewind
	 */

	UPROPERTY(EditAnywhere)
	class UBoxComponent* head;

	UPROPERTY(EditAnywhere)
	UBoxComponent* pelvis;

	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_02;

	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_03;

	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* backpack;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* blanket;

	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_r;

	UPROPERTY()
	TMap<FName, class UBoxComponent*> HitCollisionBoxes;

	UFUNCTION(Server, Reliable)
	void ServerLeaveGame();

	FOnLeftGame OnLeftGame;
	
protected:
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void ReloadButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void CalculateAO_Pitch();
	void AimOffset(float DeltaTime);
	void SimProxiesTurn();
	virtual void Jump() override;
	void FireButtonPressed();
	void FireButtonReleased();
	void PlayHitReactMontage();

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	//	Poll for any relevant classes and initialize our HUD
	void PollInit();
	void RotateInPlace(float DeltaTime);
	
private:
	UPROPERTY(VisibleAnywhere, Category=Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category=Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing=OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	/*
	 *	Sar components
	 */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* CombatComp;

	UPROPERTY(VisibleAnywhere)
	class ULagCompensationComponent* LagCompensation;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	/*
	 *	Animation montage
	 */

	UPROPERTY(EditAnywhere, Category=Combat)
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category=Combat)
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category=Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category=Combat)
	UAnimMontage* ElimMontage;

	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;
	
	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMoveReplication;
	float CalculateSpeed();

	/*
	 *	Player health
	 */

	UPROPERTY(EditAnywhere, Category="Player Stats")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category="Player Stats")
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health();

	UPROPERTY()
	class ASarPlayerController* SarPlayerController;

	bool bElimmed = false;

	FTimerHandle ElimTimer;

	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;
	
	void ElimTimerFinished();

	bool bLeftGame = false;

	/*
	 *	Dissolve effect
	 */

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;
	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;

	UFUNCTION()
	void UpdateDisolveMaterial(float DissolveValue);
	void StartDissolve();

	//	Dynamic instance that we can change at runtime
	UPROPERTY(VisibleAnywhere, Category=Elim)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	//	Material instance set on Blueprint, used with the dynamic material instance
	UPROPERTY(EditAnywhere, Category=Elim)
	UMaterialInstance* DissolveMaterialInstance;

	/*
	 *	Elim bot
	 */

	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimBotEffect;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimBotComponent;

	UPROPERTY(EditAnywhere)
	class USoundCue* ElimBotSound;

	UPROPERTY()
	class ASarPlayerState* SarPlayerState;
	
public:
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	ECombatState GetCombatState() const;
	FORCEINLINE UCombatComponent* GetCombat() const { return CombatComp; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	bool IsLocallyReloading();
	FORCEINLINE ULagCompensationComponent* GetLagCompensation() const { return LagCompensation; }
};
