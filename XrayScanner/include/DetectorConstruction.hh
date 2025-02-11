#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"

#include <vector>

class G4VPhysicalVolume;
class G4Material;
class G4VisAttributes;
class G4GenericMessenger;

/// Detector construction

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

    //Here the geomtry is constructed. This method is called only by
    //master thread and all geometry built here is shared among threads
    virtual G4VPhysicalVolume* Construct();

    //This is just a convinience: a method where all materials needed
    //are created
    void ConstructMaterials();

private:
	std::vector<G4VisAttributes*> fVisAttributes;

	G4GenericMessenger* fMessenger;
};

#endif

