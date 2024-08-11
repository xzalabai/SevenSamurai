#include "EntityGenerator.h"
#include "MissionDA.h"
#include "PaperSprite.h"
#include "GameController.h"
#include "Engine/DataTable.h"


UEntityGenerator::UEntityGenerator()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEntityGenerator::BeginPlay()
{
	Super::BeginPlay();
	GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>());
	if (GameController->MissionTypeCounts.IsEmpty())
	{
		// TODO: Do this EARLIER -> and not here, performance is going to be shit!
		const FString ContextString;
		
		TArray<const FUMissionDT*> AllMissions;
		MissionsDT->GetAllRows(ContextString, AllMissions);

		for (const FUMissionDT* Mission : AllMissions)
		{
			if (GameController->MissionTypeCounts.Contains(Mission->MissionType))
			{
				GameController->MissionTypeCounts[Mission->MissionType] += 1;
			}
			else
			{
				GameController->MissionTypeCounts.Add(Mission->MissionType, 1);
			}
		}
	}
}

UMissionDA* UEntityGenerator::GenerateMission(const uint8 AreaIndex, EMissionType MissionType) const
{
	if (MissionType == EMissionType::NotProvided)
	{
		// Generate Random mission
		const int RandomMission = FMath::RandRange(0, static_cast<int>(EMissionType::AbandonedRuins));
		MissionType = static_cast<EMissionType>(RandomMission);
	}

	const uint16 LastMissionNumber = GameController->MissionTypeCounts[MissionType];
	check(LastMissionNumber != 0);
	const int RandomNumber = FMath::RandRange(1, LastMissionNumber);
	const FName NameToFind = CustomMath::ConcatFNameAndInt(MissionTypeToFName(MissionType), RandomNumber);
	
	if (NameToFind == NAME_None)
	{
		UE_LOG(LogTemp, Error, TEXT("[AMVSevenCharacter] Wrong name for MissionType: %d"), (int)MissionType);
		return nullptr;
	}

	UMissionDA* NewMissionDA = NewObject<UMissionDA>();

	const FUMissionDT* Mission = MissionsDT->FindRow<FUMissionDT>(NameToFind, FString(), true);
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




