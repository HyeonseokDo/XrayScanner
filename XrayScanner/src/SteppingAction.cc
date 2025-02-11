#include "SteppingAction.hh"
#include "G4Step.hh"
#include "G4VPhysicalVolume.hh"
#include <stdexcept>
#include <iostream>
#include <cmath>


SteppingAction::SteppingAction(EventAction* eventAction, RunAction* runAction)
    : G4UserSteppingAction(), fEventAction(eventAction), fRunAction(runAction) {}

SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* step) {
    auto preStepPoint = step->GetPreStepPoint();
    auto postStepPoint = step->GetPostStepPoint();

    // Get position information
    G4ThreeVector prePosition = preStepPoint->GetPosition();
    G4ThreeVector postPosition = postStepPoint->GetPosition();

    // Get volume information
    const G4VPhysicalVolume* preVolume = preStepPoint->GetTouchableHandle()->GetVolume();
    const G4VPhysicalVolume* postVolume = postStepPoint->GetTouchableHandle()->GetVolume();

    // Only consider primary particle (TrackID == 1)
    if (step->GetTrack()->GetTrackID() == 1) {
		/*
		if (step->GetTrack()->GetCurrentStepNumber() == 1){
			G4int startDetectorID = postVolume->GetCopyNo();
       		// Record the start step
        	fEventAction->RecordIncidentStep(prePosition, startDetectorID);
        }
		*/
		//감마선이 실리콘으로 진입하는 순간
		if (preVolume && postVolume && preVolume->GetName() == "BreastTissue" && postVolume->GetName() == "silicon") {
			G4int incidentPixelID = postVolume->GetCopyNo();
        	fEventAction->RecordIncidentStep(prePosition, incidentPixelID);

			G4ThreeVector momentumDirection = preStepPoint->GetMomentumDirection();
			G4double theta = momentumDirection.theta();

			theta = theta * 180.0 / M_PI;

			fEventAction->RecordTheta(theta);
		}

	if (preVolume && postVolume) {
            std::string transitionKey = "";

            std::string preName = preVolume->GetName();
            std::string postName = postVolume->GetName();

            // 1. preVolume = "World", postVolume = "BreastTissue"
            if (preName == "World" && postName == "BreastTissue") {
                transitionKey = "World_to_BreastTissue";
            }
            // 2. preVolume = "BreastTissue", postVolume = "World"
            else if (preName == "BreastTissue" && postName == "World") {
                transitionKey = "BreastTissue_to_World";
            }
            // 3. preVolume = "BreastTissue", postVolume = "Silicon"
            else if (preName == "BreastTissue" && postName == "Silicon") {
                transitionKey = "BreastTissue_to_Silicon";
            }
            // 4. preVolume = "Silicon", postVolume = "World"
            else if (preName == "Silicon" && postName == "World") {
                transitionKey = "Silicon_to_World";
            }
            // 5. preVolume = "Silicon", postVolume = "BreastTissue"
            else if (preName == "Silicon" && postName == "BreastTissue") {
                transitionKey = "Silicon_to_BreastTissue";
            }
            // 6. preVolume = "World", postVolume = "OutOfWorld"
            else if (preName == "World" && postName == "OutOfWorld") {
                transitionKey = "World_to_OutOfWorld";
            }

            // 만약 전환 키가 정의되어 있다면 카운트 증가
            if (!transitionKey.empty()) {
                fRunAction->IncrementTransitionCount(transitionKey);
            }
        }

        // Record the end step when particle is killed
        if (step->GetTrack()->GetTrackStatus() == fStopAndKill) {
            G4int endDetectorID = (postVolume && postVolume->GetName() == "Silicon") ? postVolume->GetCopyNo() : -99;
			G4ThreeVector finalpostPosition = (postVolume && postVolume->GetName() == "Silicon") ? postPosition : G4ThreeVector(-999,-999,-999);

            // position, detectorID 저장 및 출력
            fEventAction->RecordFinalStep(finalpostPosition, endDetectorID);

			//검증코드
			if (endDetectorID != -99 && (endDetectorID < 0 || endDetectorID > 860 )) {
                throw std::runtime_error("invalid endDetectorID!!"+std::to_string(endDetectorID));
            }

			// Increment the stop and kill count based on the postVolume
            std::string category;
            if (postVolume != nullptr) {
                std::string volumeName = postVolume->GetName();
                if (volumeName == "Silicon" || volumeName == "World" || volumeName == "BreastTissue") {
                    category = volumeName;
                }
                else {
                    category = "WeirdCase";
                    G4cerr << "Warning: Unrecognized volume name '" << volumeName
                           << "'. Categorized as 'WeirdCase'." << G4endl;
                }
            }
            else {
                category = "OutOfWorld";
            }

            fRunAction->IncrementStopAndKillCount(category); // 카운트 증가

        }

	}
}

