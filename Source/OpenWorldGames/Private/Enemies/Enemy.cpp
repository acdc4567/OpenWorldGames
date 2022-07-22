// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemies/EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "Character/ShooterCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"


// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>("AgroSphere");
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->SetSphereRadius(800.f);

	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>("CombatSphere");
	CombatRangeSphere->SetupAttachment(GetRootComponent());
	CombatRangeSphere->SetSphereRadius(270.f);

	LeftWeaponCollision = CreateDefaultSubobject<UBoxComponent>("LeftWeaponBox");
	LeftWeaponCollision->SetupAttachment(GetMesh(), "LeftWeaponBone");
	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>("RightWeaponBox");
	RightWeaponCollision->SetupAttachment(GetMesh(), "RightWeaponBone");



}

// Called when the game starts or when spawned
void AEnemy::BeginPlay() {
	Super::BeginPlay();

	Health = MaxHealth;

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint);
	const FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint2);

	EnemyController = Cast<AEnemyController>(GetController());

	if (EnemyController) {
		EnemyController->GetBlackboardComponent()->SetValueAsBool("CanAttack", bCanAttack);
	}

	if (EnemyController) {

		EnemyController->GetBlackboardComponent()->SetValueAsVector("PatrolPoint", WorldPatrolPoint);
		EnemyController->GetBlackboardComponent()->SetValueAsVector("PatrolPoint2", WorldPatrolPoint2);
		if (BehaviorTree) {
			EnemyController->RunBehaviorTree(BehaviorTree);
		}



	}

	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnSphereOverlap);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnSphereEndOverlap);

	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnCombatSphereOverlap);
	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnCombatSphereEndOverlap);

	LeftWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnLeftWeaponOverlap);
	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnRightWeaponOverlap);




}



void AEnemy::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	if (OtherActor == nullptr)return;

	auto Character = Cast<AShooterCharacter>(OtherActor);
	if (Character&&EnemyController) {

		EnemyController->GetBlackboardComponent()->SetValueAsObject("Target", Character);

	}



}

void AEnemy::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {






}

void AEnemy::SetStunned(bool Stunned) {

	bStunned = Stunned;
	if (EnemyController) {
		EnemyController->GetBlackboardComponent()->SetValueAsBool("Stunned", Stunned);

	}



}

void AEnemy::OnCombatSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor == nullptr)return;
	auto ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
	if (ShooterCharacter == nullptr)return;
	bInAttackRange = 1;
	if (EnemyController) {

		EnemyController->GetBlackboardComponent()->SetValueAsBool("InAttackRange", 1);

	}
}

void AEnemy::OnCombatSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor == nullptr)return;
	auto ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
	if (ShooterCharacter == nullptr)return;
	bInAttackRange = 0;
	if (EnemyController) {

		EnemyController->GetBlackboardComponent()->SetValueAsBool("InAttackRange", 0);

	}

}

void AEnemy::PlayAttackMontage(FName Section, float PlayRate) {
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage) {

		AnimInstance->Montage_Play(AttackMontage, PlayRate);
		AnimInstance->Montage_JumpToSection(Section, AttackMontage);



	}
	bCanAttack = 0;
	GetWorldTimerManager().SetTimer(AttackWaitTimer, this, &AEnemy::ResetCanAttack, AttackWaitTime);
	if (EnemyController) {
		EnemyController->GetBlackboardComponent()->SetValueAsBool("CanAttack", bCanAttack);
	}



}

FName AEnemy::GetAttackSectionName() {


	
	const int32 Section = FMath::RandRange(1, 4);
	FName SectionName;
	switch (Section) {
	case 1:
		SectionName = AttackLFast;
		break;

	case 2:
		SectionName = AttackRFast;
		break;

	case 3:
		SectionName = AttackL;
		break;

	case 4:
		SectionName = AttackR;
		break;


	default:
		break;
	}



	return SectionName;
}

void AEnemy::OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	if (OtherActor == nullptr)return;

	auto Character = Cast<AShooterCharacter>(OtherActor);
	if (Character) {
		UGameplayStatics::ApplyDamage(Character, BaseDamage, EnemyController, this, UDamageType::StaticClass());
	}
	




}

void AEnemy::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor == nullptr)return;

	auto Character = Cast<AShooterCharacter>(OtherActor);
	if (Character) {
		UGameplayStatics::ApplyDamage(Character, BaseDamage, EnemyController, this, UDamageType::StaticClass());
	}


}

void AEnemy::ActivateLeftWeapon() {
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

}

void AEnemy::DeActivateLeftWeapon() {

	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::ActivateRightWeapon() {
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);


}

void AEnemy::DeActivateRightWeapon() {
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);


}

void AEnemy::ResetCanAttack() {
	bCanAttack = 1;
	if (EnemyController) {
		EnemyController->GetBlackboardComponent()->SetValueAsBool("CanAttack", bCanAttack);
	}
}

void AEnemy::FinishDeath() {
	Destroy();

}


void AEnemy::BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterController) {

	if (ImpactSound) {
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound,GetActorLocation());
	}
	if (ImpactParticles) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, HitResult.Location, HitResult.ImpactNormal.Rotation(), 1);
	}
	if (bDying)return;
	ShowHealthBar();

	const float Stunned = FMath::RandRange(0.f, 1.f);
	if (Stunned <= StunChance) {


		PlayHitMontage(FName("HitReactFront"));
		SetStunned(1);

	}



	


}

void AEnemy::ShowHealthBar_Implementation() {

	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(HealthBarTimer, this, &AEnemy::HideHealthBar, HealthBarDisplayTime);



}

void AEnemy::Die() {
	if (bDying)return;
	bDying = 1;
	HideHealthBar();
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitMontage) {
		AnimInstance->Montage_Play(HitMontage, 1.f);
		AnimInstance->Montage_JumpToSection("DeathA", HitMontage);

	}

	if (EnemyController) {
		EnemyController->GetBlackboardComponent()->SetValueAsBool("Dead", 1);
		EnemyController->StopMovement();
	}



}

void AEnemy::PlayHitMontage(FName SectionName, float PlayRate) {
	if (bCanHitReact) {
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && HitMontage) {
			AnimInstance->Montage_Play(HitMontage, PlayRate);
			AnimInstance->Montage_JumpToSection(SectionName, HitMontage);

		}
		bCanHitReact = 0;
		GetWorldTimerManager().SetTimer(HitReactTimer, this, &AEnemy::ResetHitReactTimer, 1.25f);

	}
	




}

void AEnemy::ResetHitReactTimer() {

	bCanHitReact = 1;
}







// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	
	Super::TakeDamage( DamageAmount,  DamageEvent, EventInstigator, DamageCauser);
	if (EnemyController) {
		EnemyController->GetBlackboardComponent()->SetValueAsObject("Target", DamageCauser);
	}


	if (Health - DamageAmount <= 0.f) {
		Health = 0.f;
		Die();
	}
	else {
		Health -= DamageAmount;
	}


	return DamageAmount;
}

