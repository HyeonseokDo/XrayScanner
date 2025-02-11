#ifndef STEPPING_ACTION_HH
#define STEPPING_ACTION_HH

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "EventAction.hh"
#include "RunAction.hh"

class SteppingAction : public G4UserSteppingAction {
public:
    SteppingAction(EventAction* eventAction, RunAction* runAction);
    virtual ~SteppingAction();

    virtual void UserSteppingAction(const G4Step* step) override;

private:
    EventAction* fEventAction;
    RunAction* fRunAction;
};


#endif // STEPPING_ACTION_HH
