#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#include "G4RunManagerFactory.hh"

#include "G4UImanager.hh"
#include "FTFP_BERT.hh"
#include "G4StepLimiterPhysics.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4VModularPhysicsList.hh"
#include "G4PhysListFactory.hh"

#include <iostream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
    //Detect interactive mode (if no argument) and define UI session
    G4UIExecutive* ui = nullptr;
    if ( argc == 1 ) { //No commands line argument
      //Let G4UIExecutive guess what is the best available UI
      ui = new G4UIExecutive(argc,argv);
    }

    // Construct the default run manager
    auto* runManager = G4RunManagerFactory::CreateRunManager();

    // Mandatory user initialization classes

    //====================
    //The Geometry
    runManager->SetUserInitialization(new DetectorConstruction);

    //====================
    //The Physics
    //=======================
	G4PhysListFactory physicsFactory;
	G4VModularPhysicsList* physicsList = physicsFactory.GetReferencePhysList("FTFP_BERT_EMZ");
	runManager->SetUserInitialization(physicsList);

    //====================
    // User action initialization
    runManager->SetUserInitialization(new ActionInitialization());

    // Visualization manager construction
    G4VisManager* visManager = new G4VisExecutive;
    // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
    // G4VisManager* visManager = new G4VisExecutive("Quiet");
    visManager->Initialize();

    // Get the pointer to the User Interface manager
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    if (argc>1) {
        // execute an argument macro file if exist
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command+fileName);
		std::cout<<"Batch Mode"<<std::endl;

    }
    else {
      //We have visualization, initialize defaults: look in init_vis.mac macro
      UImanager->ApplyCommand("/control/execute init_vis.mac");
      ui->SessionStart();
	  std::cout<<"Interactive Mode"<<std::endl;
      delete ui;
    }
    // Job termination
    // Free the store: user actions, physics_list and detector_description are
    // owned and deleted by the run manager, so they should not be deleted
    // in the main() program !

    delete visManager;
    delete runManager;

    return 0;
}

