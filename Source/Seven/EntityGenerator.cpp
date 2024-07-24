#include "EntityGenerator.h"
#include "MissionDA.h"
#include "PaperSprite.h"
#include "Engine/DataTable.h"

UEntityGenerator::UEntityGenerator()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEntityGenerator::BeginPlay()
{
	Super::BeginPlay();
}

UMissionDA* UEntityGenerator::GenerateMission(const uint8 AreaIndex, EMissionType MissionType) const
{
	if (MissionType == EMissionType::NotProvided)
	{
		// Generate Random mission
		const int RandomMission = FMath::RandRange(0, static_cast<int>(EMissionType::AbandonedRuins));
		MissionType = static_cast<EMissionType>(RandomMission);
	}

	const FName NameToFind = CustomMath::ConcatFNameAndFName(MissionTypeToFName(MissionType), FName(TEXT("1")));
	
	if (NameToFind == NAME_None)
	{
		UE_LOG(LogTemp, Error, TEXT("[AMVSevenCharacter] Wrong name for MissionType: %d"), (int)MissionType);
		return nullptr;
	}

	const FString ContextString;
	UMissionDA* NewMissionDA = NewObject<UMissionDA>();

	FUMissionDT* Mission = MissionsDT->FindRow<FUMissionDT>(NameToFind, ContextString, true);
	ConvertMissionToMissionDA(Mission, NewMissionDA);
	NewMissionDA->AreaIndex = AreaIndex;

	return NewMissionDA;
}

void UEntityGenerator::ConvertMissionToMissionDA(const FUMissionDT* MissionDT, UMissionDA* MissionDA) const
{
	MissionDA->UniqueID = MissionDT->UniqueID;
	MissionDA->Name = MissionDT->Name;
	MissionDA->Description = MissionDT->Description;
	MissionDA->Image = MissionDT->Image;
	MissionDA->MissionCompleteImage = MissionDT->MissionCompleteImage;
	MissionDA->MissionType = MissionDT->MissionType;
	MissionDA->EnemiesToSpawn = MissionDT->EnemiesToSpawn;
	MissionDA->Reward = MissionDT->Reward;
	MissionDA->SpecialCharacter = MissionDT->SpecialCharacter;
	MissionDA->AreaIndex = MissionDT->AreaIndex;
	MissionDA->Difficulty = MissionDT->Difficulty;
	MissionDA->MissionStatus = MissionDT->MissionStatus;
}

void UEntityGenerator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}




