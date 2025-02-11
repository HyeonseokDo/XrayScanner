#include "EventAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include <iostream>

EventAction::EventAction(RunAction* runAction)
    : G4UserEventAction(),
	  fInitPosition(G4ThreeVector()), fIncPosition(G4ThreeVector()), fFinalPosition(G4ThreeVector()),
      fIncPixelID(-1), fAbsorbPixelID(-999),fPrimaryEnergy(0.0), fTheta(-999.0), fRunAction(runAction) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* event)
{
    // Primary 에너지와 위치 저장
    if (event->GetNumberOfPrimaryVertex() > 0) {
        G4PrimaryVertex* vertex = event->GetPrimaryVertex(0);
        if (vertex->GetNumberOfParticle() > 0) {
            G4PrimaryParticle* primary = vertex->GetPrimary(0);
            fPrimaryEnergy = primary->GetKineticEnergy();
            fInitPosition = vertex->GetPosition();
            // std::cout << "Primary Gamma Energy: " << fPrimaryEnergy << " keV, "
            //           << "Position: " << fPrimaryPosition << std::endl;
        }
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event*){
    auto analysisManager = G4AnalysisManager::Instance();

    analysisManager->FillNtupleDColumn(0, fInitPosition.x());
    analysisManager->FillNtupleDColumn(1, fInitPosition.y());
    analysisManager->FillNtupleDColumn(2, fInitPosition.z());
    analysisManager->FillNtupleDColumn(3, fIncPosition.x());
    analysisManager->FillNtupleDColumn(4, fIncPosition.y());
    analysisManager->FillNtupleDColumn(5, fIncPosition.z());
    analysisManager->FillNtupleIColumn(6, fIncPixelID);

    analysisManager->FillNtupleDColumn(7, fFinalPosition.x());
    analysisManager->FillNtupleDColumn(8, fFinalPosition.y());
    analysisManager->FillNtupleDColumn(9, fFinalPosition.z());
    analysisManager->FillNtupleIColumn(10, fAbsorbPixelID);

    analysisManager->FillNtupleDColumn(11, fPrimaryEnergy);

    analysisManager->FillNtupleDColumn(12, fTheta);

    analysisManager->AddNtupleRow(0);
}

void EventAction::RecordIncidentStep(const G4ThreeVector& prePosition, G4int IncPixelID) {
    fIncPosition = prePosition;
    fIncPixelID = IncPixelID;
}

void EventAction::RecordFinalStep(const G4ThreeVector& FinalPosition, G4int AbsorbPixelID) {
    fFinalPosition = FinalPosition;
    fAbsorbPixelID = AbsorbPixelID;

}
void EventAction::RecordTheta(G4double theta) {
    if (fTheta == -999.0) { // 첫 번째 진입 시에만 기록
        fTheta = theta;
    }
}
