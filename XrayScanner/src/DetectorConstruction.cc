#include "DetectorConstruction.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4MaterialTable.hh"
#include "G4NistManager.hh"

#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4PVReplica.hh"
#include "G4UserLimits.hh"

#include "G4RunManager.hh"
#include "G4GenericMessenger.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4ios.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction(),
  fMessenger(0),
  fVisAttributes()   // fVisAttributes가 헤더에 vector<G4VisAttributes*>로 선언되어 있다고 가정
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    ConstructMaterials();
    G4Material* vacuum       = G4Material::GetMaterial("G4_Galactic");
    G4Material* silicon      = G4Material::GetMaterial("G4_Si");
    G4Material* breastTissue = G4Material::GetMaterial("BreastTissue");
    G4bool checkOverlaps     = true;

    //==============================
    // World volume
    //==============================
    G4double worldSizeX = 2 * cm;
    G4double worldSizeY = 6 * cm;
    G4double worldSizeZ = 20 * cm;
    G4VSolid* worldSolid = new G4Box("world", worldSizeX/2, worldSizeY/2, worldSizeZ/2);
    G4LogicalVolume* worldLogical = new G4LogicalVolume(worldSolid, vacuum, "worldLogical");
    G4VPhysicalVolume* worldPhysical = new G4PVPlacement(
                nullptr,
                G4ThreeVector(),
                worldLogical,
                "World",
                nullptr,
                false,
                0,
                checkOverlaps
            );

    //==============================
    // Xray Scanner (silicon detector)
    //==============================
/*    
	G4double pixelSizeX = 0.01 * cm;
    G4double pixelSizeY = 0.1  * cm;
    G4double pixelSizeZ = 7.5  * cm;

    G4int numPixelsX = 41;
    G4int numPixelsY = 21;

    G4double siliconSizeX = pixelSizeX * numPixelsX;
    G4double siliconSizeY = pixelSizeY * numPixelsY;
    G4double siliconSizeZ = pixelSizeZ;

    // 1. 전체 실리콘 디텍터 볼륨 (mother)
    G4Box* siliconS = new G4Box("silicon", siliconSizeX/2., siliconSizeY/2., siliconSizeZ/2.);
    G4LogicalVolume* siliconLV = new G4LogicalVolume(siliconS, silicon, "silicon");
	// siliconLV (실리콘 디텍터 어머니 볼륨)을 worldLogical에 배치
	new G4PVPlacement(
    	nullptr,                    // 회전 없음
    	G4ThreeVector(0,0,0),       // 원하는 위치 (예: 원점)
    	siliconLV,                  // 배치할 논리 볼륨
    	"silicon",          // 이름
    	worldLogical,               // 어머니 볼륨 (worldLogical)
    	false,
    	0,
    	checkOverlaps
	);

    // 2. X 방향 replica 생성 (슬라이스)
    //    이 슬라이스는 X 길이는 pixelSizeX, Y 길이는 실리콘 전체 길이로 구성됩니다.
    G4Box* slice = new G4Box("slice", pixelSizeX/2., siliconSizeY/2., pixelSizeZ/2.);
    G4LogicalVolume* sliceLV = new G4LogicalVolume(slice, silicon, "slice");

    // siliconLV를 X 방향으로 41개 슬라이스(sliceLV)로 분할
    new G4PVReplica("ReplicaX",      // replica 이름
                    sliceLV,         // replica로 들어갈 논리볼륨 (슬라이스)
                    siliconLV,       // 어머니 볼륨 (실리콘 디텍터)
                    kXAxis,          // 복제할 축
                    numPixelsX,      // 복제 개수 (41)
                    pixelSizeX);     // 각 replica의 두께

    // 3. 각 X 슬라이스 내부에서 Y 방향 replica 생성 (최종 pixel)
    G4Box* pixelS = new G4Box("pixel", pixelSizeX/2., pixelSizeY/2., pixelSizeZ/2.);
    G4LogicalVolume* pixelLV = new G4LogicalVolume(pixelS, silicon, "pixel");

    new G4PVReplica("ReplicaY",      // replica 이름
                    pixelLV,         // replica로 들어갈 논리볼륨 (pixel)
                    sliceLV,         // 상위 볼륨 (X 슬라이스, 즉 sliceLV)
                    kYAxis,          // 복제할 축
                    numPixelsY,      // 복제 개수 (21)
                    pixelSizeY);     // 각 replica의 두께
*/
    G4double scannerSizeX = 4.1 * mm;
    G4double scannerSizeY = 2.1 * cm;
    G4double scannerSizeZ = 7.5 * cm;
	// Pixel 크기 (각 Pixel의 크기)
	G4double pixelSizeX = 0.01 * cm;
	G4double pixelSizeY = 0.1 * cm;
	G4double pixelSizeZ = 7.5 * cm;
	// "Pixel"이라는 이름으로 G4Box 생성 (이름은 나중에 GetName() 호출 시 사용됨)
	G4Box* pixelS = new G4Box("Silicon", pixelSizeX / 2, pixelSizeY / 2, pixelSizeZ / 2);
	// 논리 볼륨 생성: 재료는 silicon (여기서는 그대로 사용)
	G4LogicalVolume* pixelLV = new G4LogicalVolume(pixelS, silicon, "Pixel");

	// Scanner(어머니 볼륨) 내부에 Pixel 볼륨을 배치
	// Scanner의 전체 크기 (이름과 변수는 DetectorConstruction에서 Scanner 볼륨을 정의한 변수 이름이어야 합니다)
	G4int numPixelsX = 41;
	G4int numPixelsY = 21;
	for (G4int ix = 0; ix < numPixelsX; ++ix) {
    	for (G4int iy = 0; iy < numPixelsY; ++iy) {
        	// Scanner의 크기(scannerSizeX, scannerSizeY)는 DetectorConstruction에서 정의한 Scanner 볼륨의 치수입니다.
        	G4double posX = -scannerSizeX/2 + (ix + 0.5) * pixelSizeX;
        	G4double posY = -scannerSizeY/2 + (iy + 0.5) * pixelSizeY;
        	new G4PVPlacement(
            	nullptr,
            	G4ThreeVector(posX, posY, 0),
            	pixelLV,
            	"Silicon",     // 배치되는 물리 볼륨 이름도 "Pixel"
            	worldLogical,   // 어머니 볼륨은 Scanner 볼륨 (scannerLV)
            	false,
            	ix * numPixelsY + iy, // 각 Pixel에 대한 copyNumber
            	checkOverlaps
        	);
    	}
}

    //==============================
    // Breast Tissue
    //==============================
    G4double tissueSizeX = 4.1 * mm;
    G4double tissueSizeY = 2.1 * cm;
    G4double tissueThickness = 1.0 * cm;  // 두께 1cm
    G4Box* tissueS = new G4Box("BreastTissue", tissueSizeX/2, tissueSizeY/2, tissueThickness/2);
    G4LogicalVolume* tissueLV = new G4LogicalVolume(tissueS, breastTissue, "BreastTissue");
    new G4PVPlacement(
        nullptr,
        G4ThreeVector(0, 0, -4.25 * cm),  // 디텍터 앞에 위치
        tissueLV,
        "BreastTissue",
        worldLogical,   // worldLogical을 어머니 볼륨으로 전달합니다.
        false,
        0,
        checkOverlaps
    );

    //==============================
    // Visualization attributes
    //==============================
    G4VisAttributes* visAttributes = new G4VisAttributes(G4Colour(1.0,1.0,1.0));
    visAttributes->SetVisibility(false);
    worldLogical->SetVisAttributes(visAttributes);
    fVisAttributes.push_back(visAttributes);

    visAttributes = new G4VisAttributes(G4Colour(0.5,1.0,1.0));
	visAttributes->SetVisibility(true);
    pixelLV->SetVisAttributes(visAttributes);
    fVisAttributes.push_back(visAttributes);

    visAttributes = new G4VisAttributes(G4Colour(1.0,0.5,0.5));
    tissueLV->SetVisAttributes(visAttributes);
    fVisAttributes.push_back(visAttributes);

    // Return the world physical volume
    return worldPhysical;
}

void DetectorConstruction::ConstructMaterials()
{
    G4NistManager* nistManager = G4NistManager::Instance();

    // Galactic
    nistManager->FindOrBuildMaterial("G4_Galactic");

    // Silicon
    nistManager->FindOrBuildMaterial("G4_Si");

    // Breast Tissue
    G4double density = 1.02 * g/cm3;
    G4Material* breastTissue = new G4Material("BreastTissue", density, 8);
    breastTissue->AddElement(nistManager->FindOrBuildElement("H"), 10.6 * perCent);
    breastTissue->AddElement(nistManager->FindOrBuildElement("C"), 33.2 * perCent);
    breastTissue->AddElement(nistManager->FindOrBuildElement("N"), 3.0 * perCent);
    breastTissue->AddElement(nistManager->FindOrBuildElement("O"), 52.7 * perCent);
    breastTissue->AddElement(nistManager->FindOrBuildElement("Na"), 0.1 * perCent);
    breastTissue->AddElement(nistManager->FindOrBuildElement("P"), 0.1 * perCent);
    breastTissue->AddElement(nistManager->FindOrBuildElement("S"), 0.2 * perCent);
    breastTissue->AddElement(nistManager->FindOrBuildElement("Cl"), 0.1 * perCent);

    G4cout << G4endl << "The materials defined are : " << G4endl << G4endl;
    G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}
