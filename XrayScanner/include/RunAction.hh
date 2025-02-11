#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include <map>
#include <string>
#include <mutex>

class G4Run;

/// Run action class

class RunAction : public G4UserRunAction
{
  public:
    RunAction();
    virtual ~RunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);

    void IncrementStopAndKillCount(const std::string& category);
    void PrintStopAndKillCounts() const;

    void IncrementTransitionCount(const std::string& transitionKey);
    void PrintTransitionCounts() const;

    // Merge worker RunAction counts into this (master) RunAction
    void Merge(const RunAction* workerRunAction);
    // Static method for worker threads to merge their counts into master
    static void MergeWorkerRunAction(const RunAction* workerRunAction);

  private:
    std::map<std::string, int> fStopAndKillCounts;
    std::map<std::string, int> fTransitionCounts;

    // Static pointer to the master RunAction
    static RunAction* masterRunAction;
    // Mutex for thread-safe merging
    static std::mutex mergeMutex;
	
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
