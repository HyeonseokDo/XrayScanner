#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

#include <random>
#include <vector>
#include <string>
#include <fstream>


class G4ParticleGun;
class G4ParticleDefinition;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction();
    virtual ~PrimaryGeneratorAction();

    virtual void GeneratePrimaries(G4Event*) override;

private:
	G4ParticleGun* fParticleGun;
	G4ParticleDefinition* fGamma;

	// 랜덤 엔진 및 분포
    std::mt19937 generator;
    std::uniform_int_distribution<> distribution;

	std::vector<double> sampledEnergies;
	void LoadEnergiesFromFile(const std::string& filename);
};

#endif
