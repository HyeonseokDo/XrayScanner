#include "RunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4ios.hh"
#include <iostream>
#include <mutex>

// 초기화: masterRunAction는 nullptr로, 그리고 뮤텍스 객체 생성
RunAction* RunAction::masterRunAction = nullptr;
std::mutex RunAction::mergeMutex;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction()
 : G4UserRunAction()
{
    //for validatoin
    fStopAndKillCounts["Silicon"] = 0;
    fStopAndKillCounts["World"] = 0;
    fStopAndKillCounts["BreastTissue"]=0;
    fStopAndKillCounts["OutOfWorld"] = 0;
    fStopAndKillCounts["WeirdCase"] = 0;

    // TransitionCounts 초기화
    fTransitionCounts["World_to_BreastTissue"] = 0;
    fTransitionCounts["BreastTissue_to_World"] = 0;
    fTransitionCounts["BreastTissue_to_Silicon"] = 0;
    fTransitionCounts["Silicon_to_World"] = 0;
    fTransitionCounts["Silicon_to_BreastTissue"] = 0;
    fTransitionCounts["World_to_OutOfWorld"] = 0;

    // 만약 masterRunAction이 아직 설정되지 않았다면, 이 인스턴스를 master로 지정
    std::lock_guard<std::mutex> lock(mergeMutex);
    if (!masterRunAction) {
        masterRunAction = this;
    }

    auto analysisManager = G4AnalysisManager::Instance();
    if (!analysisManager) {
        G4cerr << "Failed to create G4AnalysisManager!" << G4endl;
        return;
    }
    // Default settings
    analysisManager->SetDefaultFileType("root");
    G4cout << "Using " << analysisManager->GetType() << G4endl;
    analysisManager->SetVerboseLevel(1);
    analysisManager->SetFileName("Analysis");
    analysisManager->SetNtupleMerging(true);//다중스레드에서 생성된 정보를 하나의 Ntuple에 저장가능

    // Creating ntuple
    //
    analysisManager->CreateNtuple("Analysis","Info");
    analysisManager->CreateNtupleDColumn("InitX");
    analysisManager->CreateNtupleDColumn("InitY");
    analysisManager->CreateNtupleDColumn("InitZ");

    analysisManager->CreateNtupleDColumn("IncidentX");
    analysisManager->CreateNtupleDColumn("IncidentY");
    analysisManager->CreateNtupleDColumn("IncidentZ");
    analysisManager->CreateNtupleIColumn("IncidentPixelID");

    analysisManager->CreateNtupleDColumn("FinalX");
    analysisManager->CreateNtupleDColumn("FinalY");
    analysisManager->CreateNtupleDColumn("FinalZ");
    analysisManager->CreateNtupleIColumn("AbsorbPixelID");

    analysisManager->CreateNtupleDColumn("Energy_Spectrum");

    analysisManager->CreateNtupleDColumn("Theta");

 }
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run*)
{
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();
  // Open an output file
  // The default file name is set in RunAction::RunAction(),
  // it can be overwritten in a macro
  analysisManager->OpenFile();
}

void RunAction::EndOfRunAction(const G4Run* run)
{
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();

    // 만약 이 RunAction이 master가 아니라면, master에 자신의 카운터를 병합
    if (this != masterRunAction) {
        MergeWorkerRunAction(this);
    }
    else {
        // master인 경우 최종 병합 결과를 출력
        PrintStopAndKillCounts();
        PrintTransitionCounts();
    }
}

void RunAction::IncrementStopAndKillCount(const std::string& category) {
    if (fStopAndKillCounts.find(category) != fStopAndKillCounts.end()) {
        fStopAndKillCounts[category]++;
    }
    else {
        // 새로운 카테고리가 발견되면 "WeirdCase"로 매핑
        fStopAndKillCounts["WeirdCase"]++;
        G4cerr << "Warning: Unrecognized category '" << category << "'. Mapped to 'WeirdCase'." << G4endl;
    }
}

void RunAction::IncrementTransitionCount(const std::string& transitionKey) {
    if (fTransitionCounts.find(transitionKey) != fTransitionCounts.end()) {
        fTransitionCounts[transitionKey]++;
    } else {
        // 정의되지 않은 전환 키는 "WeirdCase"로 매핑
        fTransitionCounts["WeirdCase"]++;
        G4cerr << "Warning: Unrecognized transition key '" << transitionKey << "'. Mapped to 'WeirdCase'." << G4endl;
    }
}

void RunAction::PrintStopAndKillCounts() const {
    std::cout << "Total StopAndKill counts:" << std::endl;
    for (const auto& entry : fStopAndKillCounts) {
        std::cout << entry.first << ": " << entry.second << std::endl;
    }
}

void RunAction::PrintTransitionCounts() const {
    std::cout << "Total Transition counts:" << std::endl;
    for (const auto& entry : fTransitionCounts) {
        std::cout << entry.first << ": " << entry.second << std::endl;
    }
}

void RunAction::Merge(const RunAction* workerRunAction)
{
    std::lock_guard<std::mutex> lock(mergeMutex);
    // 각 워커의 StopAndKill 카운터를 master에 합산
    for (const auto& entry : workerRunAction->fStopAndKillCounts) {
        fStopAndKillCounts[entry.first] += entry.second;
    }
    // 각 워커의 Transition 카운터를 master에 합산
    for (const auto& entry : workerRunAction->fTransitionCounts) {
        fTransitionCounts[entry.first] += entry.second;
    }
}

void RunAction::MergeWorkerRunAction(const RunAction* workerRunAction)
{
    std::lock_guard<std::mutex> lock(mergeMutex);
    if (masterRunAction) {
        masterRunAction->Merge(workerRunAction);
    }
}

