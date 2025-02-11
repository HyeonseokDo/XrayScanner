#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(0),
  fGamma(0)
{
	G4int n_particle = 1;
	fParticleGun = new G4ParticleGun(n_particle);
	
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName;
	fGamma = particleTable->FindParticle(particleName="gamma");
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));

	// default particle kinematics
    fParticleGun->SetParticleDefinition(fGamma);

    // Random Engine initialization 
    //std::random_device rd;
    generator = std::mt19937(42); // seed 고정 안할꺼면  std::mt19937(rd());

    // 에너지 데이터 파일 읽기
    std::string energyFile = "sampled_energies.txt"; // 파일 경로 조정 필요 시 수정
    LoadEnergiesFromFile(energyFile);

    if (sampledEnergies.empty()) {
        G4cerr << "Error: No energy data loaded from " << energyFile << "!" << G4endl;
        exit(1);
    }

    // 샘플링을 위한 인덱스 분포 설정
    distribution = std::uniform_int_distribution<>(0, sampledEnergies.size() - 1);
	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::LoadEnergiesFromFile(const std::string& filename)
{
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        G4cerr << "Error: Cannot open energy file " << filename << "!" << G4endl;
        exit(1);
    }

    double energy;
    while (infile >> energy) {
        sampledEnergies.push_back(energy);
    }
    infile.close();

    if (sampledEnergies.empty()) {
        G4cerr << "Error: No energies found in file " << filename << "!" << G4endl;
        exit(1);
    }

    G4cout << "Loaded " << sampledEnergies.size() << " energies from " << filename << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
    G4double SizeX = 4.1 * mm;
    G4double SizeY = 2.1 * cm;

    G4double x0 = SizeX * (G4UniformRand() - 0.5);
    G4double y0 = SizeY * (G4UniformRand() - 0.5);
    G4double z0 = -7. * cm;

    // 샘플링된 에너지 얻기
    int index = distribution(generator);
    G4double randomEnergy = sampledEnergies[index] * keV;
	fParticleGun->SetParticleEnergy(randomEnergy);

	fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));
    fParticleGun->GeneratePrimaryVertex(event);
}
