#include "B4cCalorimeterSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4UnitsTable.hh"
#include "G4VProcess.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cCalorimeterSD::B4cCalorimeterSD(
                                   const G4String& name,
                                   const G4String& hitsCollectionName,
                                   G4int nofCells)
: G4VSensitiveDetector(name),
fHitsCollection(nullptr),
fNofCells(nofCells)
{
    collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cCalorimeterSD::~B4cCalorimeterSD() 
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cCalorimeterSD::Initialize(G4HCofThisEvent* hce)
{
    // Create hits collection
    fHitsCollection
    = new B4cCalorHitsCollection(SensitiveDetectorName, collectionName[0]);
    
    // Add this collection in hce
    auto hcID
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    hce->AddHitsCollection( hcID, fHitsCollection );
    
    // Create hits
    // fNofCells for cells + one more for total sums
    for (G4int i=0; i<fNofCells+1; i++ ) {
        fHitsCollection->insert(new B4cCalorHit());
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool B4cCalorimeterSD::ProcessHits(G4Step* step, 
                                     G4TouchableHistory*)
{  
    // energy deposit
    auto edep = step->GetTotalEnergyDeposit();
    
    // step length
    G4double stepLength = 0.;
    stepLength = step->GetStepLength();

    if ( edep==0. && stepLength == 0. ) return false;
    auto touchable = (step->GetPreStepPoint()->GetTouchable());
    
    // Get calorimeter cell id
    auto layerNumber = touchable->GetReplicaNumber(1);
    
    // Get hit accounting data for this cell
    auto hit = (*fHitsCollection)[layerNumber];
    if ( ! hit ) {
        G4ExceptionDescription msg;
        msg << "Cannot access hit " << layerNumber;
        G4Exception("B4cCalorimeterSD::ProcessHits()",
                    "MyCode0004", FatalException, msg);
    }
    
     //Get hit for total accounting
    auto hitTotal
    = (*fHitsCollection)[fHitsCollection->entries()-1];
    
    // Add values
    hit->Add(edep, stepLength);
    hitTotal->Add(edep, stepLength);
    
    // Qui prende la definizione della particle ID (pid)
    auto pid = step->GetTrack()->GetDefinition()->GetParticleName();
    auto volumeName = step->GetPreStepPoint()->GetPhysicalVolume()->GetName();

    // Questa funzione controlla se la traccia passa dal volume xtal al volume pmt
    if (volumeName == "Detector"){
        G4bool isNeutronInDetector = Mcross(step, "Detector", "World");
        if (pid == "neutron" && isNeutronInDetector) {

            G4double neutronE = step->GetPreStepPoint()->GetKineticEnergy();
            G4ThreeVector pos = step->GetPreStepPoint()->GetPosition();
            hit->AddNeutronInDetector(neutronE);
            hit->AddPositionInDetector(pos.x(), pos.y());
            hitTotal->AddNeutronInDetector(neutronE);
            hitTotal->AddPositionInDetector(pos.x(), pos.y());
            Kill(step);
        }
    }
///////////////////////////////////////
    if (volumeName == "Room") {
        ///////////////////////////////////////////7
        //G4ThreeVector pos = step->GetPostStepPoint()->GetPosition();
        //G4cout << "Track length: " << step->GetTrack()->GetDefinition()->GetParticleName() << G4BestUnit(step->GetTrack()->GetStepLength(), "Length")<< "position" <<G4BestUnit(pos.z(),"Length") << G4endl;
        //G4cout << "Energy : " << G4BestUnit(step->GetTrack()->GetKineticEnergy(),"Energy")  << G4endl;
        //G4cout << "GetPreStepPoint Energy=" << G4BestUnit(step->GetPreStepPoint()->GetKineticEnergy(),"Energy") << G4endl;
        //G4cout << "GetPostStepPoint Energy=" << G4BestUnit(step->GetPostStepPoint()->GetKineticEnergy(),"Energy") << G4endl;
        //G4cout << "Process: " << step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() << G4endl;
       // auto creatorProcess = step->GetTrack()->GetCreatorProcess();
        //if (creatorProcess) {
          //  G4cout << "Creator Process: " << creatorProcess->GetProcessName() << G4endl;
        //}

        ////////////////////////////////////////7
        //Saving energy and position of neutron that crosses boundary
        G4bool isNeutronOutOfRoom = Mcross(step, "Room", "World");
        if (pid == "neutron" && isNeutronOutOfRoom) {

            G4double neutronE = step->GetPostStepPoint()->GetKineticEnergy();
            G4ThreeVector pos = step->GetPostStepPoint()->GetPosition();
            hit->AddBoundaryTracking(pos, neutronE);
            hitTotal->AddBoundaryTracking(pos, neutronE);
            //G4cout << "Neutron out of volume, Energy=" << G4BestUnit(neutronE,"Energy") << G4endl;
        }


        /////////////////////////////////////////////////////////
        ///////////////////// SECONDARIES ///////////////////////
        ////////////////////////////////////////////////////////

        // Get the secondary particles produced in the current step
        //const std::vector<const G4Track*>* secondaries = step->GetSecondaryInCurrentStep();
        G4int trackID = step->GetTrack()->GetTrackID();

        // Loop over the secondary particles
        if ( trackID!=1 ){
            G4double particleEnergy = step->GetTrack()->GetKineticEnergy();
            G4int atomicNumber = step->GetTrack()->GetDefinition()->GetAtomicNumber();
            G4int atomicMass = step->GetTrack()->GetDefinition()->GetAtomicMass();
            G4double secondaryDepositedEnergy = step->GetTotalEnergyDeposit();
            G4double secondaryTrackLength = step->GetTrack()->GetTrackLength();
            G4ThreeVector pos = step->GetPostStepPoint()->GetPosition();
            hit->AddSecondaryParticle(trackID, atomicNumber, atomicMass, secondaryDepositedEnergy, secondaryTrackLength);
            hitTotal->AddSecondaryParticle(trackID, atomicNumber, atomicMass, secondaryDepositedEnergy, secondaryTrackLength);

            if (atomicNumber == 18 && atomicMass == 41){
                hit->AddAr41(trackID, secondaryDepositedEnergy, pos);
                hitTotal->AddAr41(trackID, secondaryDepositedEnergy, pos);
                //G4cout << "Particle name"<< pid << trackID << " particle energy " << G4BestUnit(secondaryDepositedEnergy,"Energy") <<"x " << G4BestUnit(pos.x(),"Length") << "y "<< G4BestUnit(pos.y(),"Length") << "z " <<G4BestUnit(pos.z(),"Length") << G4endl;
            }

            /*
            for (size_t i = 0; i < secondaries->size(); ++i) {
                const G4Track *secondaryTrack = (*secondaries)[i];
                // Get the particle definition of the secondary particle
                G4ParticleDefinition *particle = secondaryTrack->GetDefinition();
                G4double particleEnergy = secondaryTrack->GetKineticEnergy();
                G4String particleName = particle->GetParticleName();
                // Now you can access information about the secondary particle, for example its name
                G4int Z = (*secondaries)[i]->GetDefinition()->GetAtomicNumber();
                G4int A = (*secondaries)[i]->GetDefinition()->GetAtomicMass();

                const G4VProcess* creatorProcess = secondaryTrack->GetCreatorProcess();

                if(creatorProcess->GetProcessName() != "hadElastic"){
                    particleEnergies.push_back(particleEnergy);
                    AtomicNumbers.push_back(Z);
                    AtomicMasses.push_back(A);
                }
*/
                //if(creatorProcess->GetProcessName() == "hadElastic"){
                   // G4cout << "Particle name"<< particleName << " particle energy " << G4BestUnit(particleEnergy,"Energy") <<"x " << G4BestUnit(pos.x(),"Length") << "y "<< G4BestUnit(pos.y(),"Length") << "z " <<G4BestUnit(pos.z(),"Length") << G4endl;
               // }



            //hit->AddSecondaryParticle(particleEnergies, AtomicMasses, AtomicNumbers);
            //hitTotal->AddSecondaryParticle(particleEnergies, AtomicMasses, AtomicNumbers);
/*
            for (size_t i = 0; i < particleEnergies.size(); ++i){
                G4cout << "AAA particleEnergies " << particleEnergies[i] << G4endl;
                G4cout << "AAA AtomicMasses " << AtomicMasses[i] << G4endl;
                G4cout << "AAA AtomicNumbers " << AtomicNumbers[i] << G4endl;

            }
            G4cout << "___________________" << G4endl;
*/
        }

    }


    /*
    // Get the secondary particles generated in this step
    const std::vector<const G4Track*>* secondary = step->GetSecondaryInCurrentStep();

    // Loop through the secondary particles
    for (size_t i = 0; i < secondary->size(); ++i) {
        // Get the particle ID of each secondary particle
        auto secondaryName = (*secondary)[i]->GetDefinition()->GetParticleName();
        auto secondaryEnergy = (*secondary)[i]->GetKineticEnergy();
        G4int Z = (*secondary)[i]->GetDefinition()->GetAtomicNumber();
        G4int A = (*secondary)[i]->GetDefinition()->GetAtomicMass();

        // Now you can use secondaryPid or secondaryName as needed
         
      //  G4cout << "Prove" << secondary->size() << i << G4endl;
// Retrieve the step number
//G4int stepID = step->GetTrack()->GetCurrentStepNumber();

// Print step ID
//G4cout << "Step ID: " << stepID << G4endl;


        if (secondaryName == "Ar41") { //proveArgon
            hit->AddAr41(secondaryEnergy);
            hitTotal->AddAr41(secondaryEnergy);
            G4cout << "the Secondary Particle: " << secondaryName << " Has atomic mass " << A <<  " and atomic number " << Z << "Prove" << secondary->size() << i << G4endl;

            //Kill(step);
        }
	if (Z<1){
            hit->AddSecondaryParticle(secondaryEnergy, A, Z);
            hitTotal->AddSecondaryParticle(secondaryEnergy, A, Z);
            // G4cout << "the Secondary Particle: " << secondaryName << " Has atomic mass " << A << " and atomic number " << Z << "Prove" << secondary->size() << i << stepID << G4endl;
	}
    }
    */
    ///////////////////////


    return true;
}


void B4cCalorimeterSD::Kill(G4Step *step){
    step->GetTrack()->SetTrackStatus(fStopAndKill);
}

//returns true if the particle crosses the boundary between vol1 and vol2
//Note: use the Physical Volumes names!
bool B4cCalorimeterSD::Mcross(const G4Step* aStep, const G4String& vol1_name, const G4String& vol2_name){
    G4bool did_cross=false;
    if (aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName()==vol1_name && aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName()==vol2_name){
        did_cross = true;
    }
    return did_cross;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cCalorimeterSD::EndOfEvent(G4HCofThisEvent*)
{
    if ( verboseLevel>1 ) {
        G4int nofHits = fHitsCollection->entries();
        G4cout
        << G4endl
        << "-------->Hits Collection: in this event they are " << nofHits
        << " hits in the tracker chambers: " << G4endl;
        for ( G4int i=0; i<nofHits; i++ ) (*fHitsCollection)[i]->Print();
    }
}
