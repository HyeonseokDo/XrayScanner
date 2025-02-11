#ifndef EventAction_h
#define EventAction_h 1


#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

class RunAction;    

/// Event action

class EventAction : public G4UserEventAction
{
public:
    EventAction(RunAction* runAction);
    virtual ~EventAction();

    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);

    void RecordIncidentStep(const G4ThreeVector& prePosition, G4int startDetectorID);
    void RecordFinalStep(const G4ThreeVector& endPosition, G4int endDetectorID);

    void RecordTheta(G4double theta);

private:
	G4ThreeVector fInitPosition;
	G4ThreeVector fIncPosition;
    G4ThreeVector fFinalPosition;
    G4int fIncPixelID;
    G4int fAbsorbPixelID;

    G4double fPrimaryEnergy;

	G4double fTheta;

    RunAction* fRunAction;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
