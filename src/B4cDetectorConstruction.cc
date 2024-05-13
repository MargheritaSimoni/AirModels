#include "B4cDetectorConstruction.hh"
#include "B4cCalorimeterSD.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"
#include "G4SubtractionSolid.hh"

#include "G4SDManager.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4NCrystal/G4NCrystal.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal
G4GlobalMagFieldMessenger* B4cDetectorConstruction::fMagFieldMessenger = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cDetectorConstruction::B4cDetectorConstruction()
: G4VUserDetectorConstruction(),
fCheckOverlaps(true),
fNofLayers(-1)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cDetectorConstruction::~B4cDetectorConstruction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B4cDetectorConstruction::Construct()
{
    // Define materials
    DefineMaterials();

    // Define volumes
    return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cDetectorConstruction::DefineMaterials()
{
    G4double airDensity = 0.001225  * g/cm3;
    G4double temperature=293.6*kelvin; //nb.: this is only relevant for geant, NC has its own temperature (defined in the file or in MatHelper?), so the two have to be the same

    //VACUUM
    G4double z, a, density;
    new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                   kStateGas, 2.73*kelvin, 3.e-18*pascal);


    //AIR FREE GAS MODEL with geant4
    auto nistManager = G4NistManager::Instance();
 // elements
    //G4Element* elAr = nistManager->FindOrBuildElement("Ar"); // this way they contain multiple isotopes, i prefer to use only the most abundant
    //G4Element* elO = nistManager->FindOrBuildElement("O");
    //G4Element* elN = nistManager->FindOrBuildElement("N");
    //G4Element* elH = nistManager->FindOrBuildElement("H");
    G4String name, symbol;
    G4Isotope* isoN14 = new G4Isotope("Nitrogen-14", 7, 14, 14.007*g/mole);
    G4Element* elN = new G4Element("Nitrogen", "N", 1);
    elN->AddIsotope(isoN14, 1.0);

    G4Isotope* isoO16 = new G4Isotope("Oxygen-16", 8, 16, 15.999*g/mole);
    G4Element* elO = new G4Element("Oxygen", "O", 1);
    elO->AddIsotope(isoO16, 1.0);

    G4Isotope* isoAr40 = new G4Isotope("Argon-40", 18, 40, 39.948*g/mole);
    G4Element* elAr = new G4Element("Argon", "Ar", 1);
    elAr->AddIsotope(isoAr40, 1.0);

    G4Isotope* isoH1 = new G4Isotope("Hydrogen-1", 1, 1, 1.008*g/mole);
    G4Element* elH = new G4Element("Hydrogen", "H", 1);
    elH->AddIsotope(isoH1, 1.0);

    // Dry Air defined by me, using fraction of mass
    //nb.: Geant4 interprets the argument as weight percentage, it only takes numbers between 0 and 1
    G4double fractionOfMass; //G4 wants material composition by number of elements or mass fraction
    G4Material* fG4air = new G4Material("AirDry_G4", airDensity, 3,kStateGas,temperature); // 3 is the number of components (3 elements)
    fG4air->AddElement(elN, fractionOfMass=0.7555592382447177);
    fG4air->AddElement(elO, fractionOfMass=0.23147437550371744);
    fG4air->AddElement(elAr,fractionOfMass= 0.01296638625156488);

    G4Material* fG4airHy = new G4Material("AirHydr40_G4", airDensity, 4,kStateGas,temperature); // 3 is the number of components (3 elements)
    fG4airHy->AddElement(elN, fractionOfMass=0.7549326791598274);
    fG4airHy->AddElement(elO, fractionOfMass=0.23128242182814845);
    fG4airHy->AddElement(elAr,fractionOfMass= 0.012955633676924953);
    fG4airHy->AddElement(elH,fractionOfMass= 0.0008292653350992586 );

    //AIR DEFINED USING NCRYSTAL LIBRARIES
    //nb: NC has a standard temperature of 293.15 instead of 273.15 that is set as standard in G4
    //nb.: NC takes the density and temperature from .ncmat the file
    //nb.: NC does not use mass fraction, it uses mole fraction of the element
    G4Material * fairNC = G4NCrystal::createMaterial("myDryAir.ncmat");
    fairNC->SetName("AirDry_NC");


    G4Material * fairHydNC = G4NCrystal::createMaterial("myHydrAir_50pc.ncmat");
    fairHydNC->SetName("AirHydr50_NC");

    // GEANT4 AIR
    //nistManager->FindOrBuildMaterial("G4_AIR");// non scattera, perché?

    // Print materials
    G4cout << *(G4Material::GetMaterialTable()) << G4endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B4cDetectorConstruction::DefineVolumes()
{
    // Geometry parameters

    // world
    G4double worldSizeXY = 3.5*m; // if this is changed the starting point in primary generator of the beam should be changed as well
    G4double worldSizeZ  = 3.5*m;

    // room
    G4double roomXY = 3*m;
    G4double roomZ  = 3*m;


    //detector
    G4double detectorXY =  1*m;//worldSizeXY-1*mm;
    G4double detectorZ =  0.5*mm;
          //air layer in front of the detector used to count neutrons
    G4double airLayerXY =  detectorXY;
    G4double airLayerZ =  detectorZ;
    //worldSizeZ=worldSizeZ+2*detectorZ;


    G4double roomPosition = worldSizeZ/2.-roomZ/2.-1*cm; // last number distance from the world wall
    G4double detectorPosition= roomZ/2.- roomPosition +0.5*mm;//last number distance room-detector


/*
    G4double dimensioneProvaXY=detectorXY;
    G4double dimensioneProvaZ=2.*mm;
*/

    fNofLayers = 1;

    // Get materials

auto air = G4Material::GetMaterial("AirDry_G4");
    auto testMaterial = G4Material::GetMaterial("Galactic");

    if ( !air || !testMaterial) {
        G4ExceptionDescription msg;
        msg << "Cannot retrieve materials already defined.";
        G4Exception("B4DetectorConstruction::DefineVolumes()",
                    "MyCode0001", FatalException, msg);
    }


//
    // World
    //
    auto worldS
            = new G4Box("World",           // its name
                        worldSizeXY/2, worldSizeXY/2, worldSizeZ/2); // its size

    auto worldLV
            = new G4LogicalVolume(
                    worldS,           // its solid
                    testMaterial,  // its material
                    "World");         // its name

    auto worldPV
            = new G4PVPlacement(
                    0,                // no rotation
                    G4ThreeVector(),  // at (0,0,0)
                    worldLV,          // its logical volume
                    "World",          // its name
                    0,                // its mother  volume
                    false,            // no boolean operation
                    0,                // copy number
                    fCheckOverlaps);  // checking overlaps

    //
    // Detector
    //

    auto detectorS
            = new G4Box("Detector",     // its name
                        detectorXY/2, detectorXY/2, detectorZ/2); // its size

    auto detectorLV
            = new G4LogicalVolume(
                    detectorS,     // its solid
                    testMaterial,  // its material
                    "detectorLV");   // its name

    new G4PVPlacement(
            0,                // no rotation
            G4ThreeVector(0,0,detectorPosition+detectorZ/2.),  // at (0,0,0)
            detectorLV,          // its logical volume
            "Detector",    // its name
            worldLV,          // its mother  volume
            false,            // no boolean operation
            0,                // copy number
            fCheckOverlaps);  // checking overlaps

    //
    // air layer for detector
    //

    auto airLayerS
            = new G4Box("AirLayer",             // its name
                        airLayerXY/2, airLayerXY/2, airLayerZ/2); // its size

    auto airLayerLV
            = new G4LogicalVolume(
                    airLayerS,             // its solid
                    testMaterial,      // its material
                    "airLayerLV");         // its name
    new G4PVPlacement(
            0,                // no rotation
            G4ThreeVector(0,0,detectorPosition-airLayerZ/2.), // its position
            airLayerLV,            // its logical volume
            "AirLayer",            // its name
            worldLV,          // its mother  volume
            false,            // no boolean operation
            0,                // copy number
            fCheckOverlaps);  // checking overlaps


    auto RoomS
            = new G4Box("Room",             // its name
                        roomXY/2, roomXY/2, roomZ/2); // its size

    auto RoomLV
            = new G4LogicalVolume(
                    RoomS,             // its solid
                    air,      // its material
                    "RoomLV");         // its name
    new G4PVPlacement(
            0,                // no rotation
            G4ThreeVector(0,0,-roomPosition), // its position
            RoomLV,            // its logical volume
            "Room",            // its name
            worldLV,          // its mother  volume
            false,            // no boolean operation
            0,                // copy number
            fCheckOverlaps);  // checking overlaps

    ////////////////////////////////////////////////////////////////////////////

/*
    auto VolumeProvaS
            = new G4Box("VolumeProva",             // its name
                        dimensioneProvaXY/2, dimensioneProvaXY/2, dimensioneProvaZ/2); // its size

    auto VolumeProvaLV
            = new G4LogicalVolume(
                    VolumeProvaS,             // its solid
                    air,      // its material
                    "VolumeProvaLV");         // its name
    new G4PVPlacement(
            0,                // no rotation
            G4ThreeVector(0,0,0), // its position
            VolumeProvaLV,            // its logical volume
            "VolumeProva",            // its name
            worldLV,          // its mother  volume
            false,            // no boolean operation
            0,                // copy number
            fCheckOverlaps);  // checking overlaps

*/

    //
    // Visualization attributes
    //
    worldLV->SetVisAttributes (G4VisAttributes::GetInvisible());

    // Set transparency attributes
    G4VisAttributes* visAttributesDetector = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0, 0.5)); // Red color with 50% transparency
    visAttributesDetector->SetForceWireframe(true); // Display wireframe
    visAttributesDetector->SetForceSolid(true);
    detectorLV->SetVisAttributes(visAttributesDetector);


    //
    // Always return the physical World
    //
    return worldPV;

}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cDetectorConstruction::ConstructSDandField()
{
    // G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

    //
    // Sensitive detectors
    //
    //qui creo solo il sensitive detector ma in realtà non lo uso poi
    auto detectorSD
    = new B4cCalorimeterSD("detectorSD", "detectorHitsCollection", fNofLayers);
    G4SDManager::GetSDMpointer()->AddNewDetector(detectorSD);
    SetSensitiveDetector("detectorLV",detectorSD);

    //qui invece assegno il sensitive detector
    auto airSD
    = new B4cCalorimeterSD("airSD", "airLayerHitsCollection", fNofLayers);
    G4SDManager::GetSDMpointer()->AddNewDetector(airSD);
    SetSensitiveDetector("airLayerLV",airSD);

    auto roomSD
            = new B4cCalorimeterSD("roomSD", "roomHitsCollection", fNofLayers);
    G4SDManager::GetSDMpointer()->AddNewDetector(roomSD);
    SetSensitiveDetector("RoomLV",roomSD);

    //
    // Magnetic field
    //
    // Create global magnetic field messenger.
    // Uniform magnetic field is then created automatically if
    // the field value is not zero.
    G4ThreeVector fieldValue;
    fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
    fMagFieldMessenger->SetVerboseLevel(1);

    // Register the field messenger for deleting
    G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
