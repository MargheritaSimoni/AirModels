#include "B4cEventAction.hh"
#include "B4cCalorimeterSD.hh"
#include "B4cCalorHit.hh"
#include "B4Analysis.hh"
#include "B4PrimaryGeneratorAction.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>
#include "G4Track.hh"

//NOTA IMPORTANTE: abso qui è la collezione di hit nel PMT, xtal nel cristallo come puoi vedere a riga 79

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cEventAction::B4cEventAction()
        : G4UserEventAction(),
          fdetectorHCID(-1),
          fRoomHCID(-1)//proveArgon
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cEventAction::~B4cEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cCalorHitsCollection*
B4cEventAction::GetHitsCollection(G4int hcID,
                                  const G4Event* event) const
{
    auto hitsCollection
            = static_cast<B4cCalorHitsCollection*>(
                    event->GetHCofThisEvent()->GetHC(hcID));

    if ( ! hitsCollection ) {
        G4ExceptionDescription msg;
        msg << "Cannot access hitsCollection ID " << hcID;
        G4Exception("B4cEventAction::GetHitsCollection()",
                    "MyCode0003", FatalException, msg);
    }

    return hitsCollection;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cEventAction::PrintEventStatistics(
        G4double nNeutrons, G4double eNeutrons,
        //G4double Edep,
        //G4double nAr41, G4double eAr41,
        //G4double eSec, G4int nA, G4int nZ,
        G4double xPos, G4double yPos, G4ThreeVector gp) const //proveArgon
{
    // print event statistics
    G4cout
            << "   Neutrons in the detector: "
            << std::setw(7) << nNeutrons
            << G4endl
            << "       neutron energy: "
            << std::setw(7) << G4BestUnit(eNeutrons, "Energy")
            << G4endl
            //<< "        total energy: "
            //<< std::setw(7) << G4BestUnit(Edep, "Energy")
            //<< G4endl
            //<< "        Number of Argon 41 "//proveArgon
            //<< std::setw(7) << nAr41
            //<< G4endl
            //<< "        Energy of Argon 41 "//proveArgon
            //<< std::setw(7) << eAr41
            //<< G4endl
            << "        X position: "
            << std::setw(7) << G4BestUnit(xPos, "Length")
            << G4endl
            << "        Y position: "
            << std::setw(7) << G4BestUnit(yPos, "Length")
            << G4endl
            <<"Generator Position: "
            <<gp[0]<<" "<<gp[1]<<" "<<gp[2]
            <<G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cEventAction::BeginOfEventAction(const G4Event* /*event*/)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cEventAction::EndOfEventAction(const G4Event* event)
{
    // Get hits collections IDs (only once)
    if ( fdetectorHCID == -1 ) {
        fdetectorHCID
                = G4SDManager::GetSDMpointer()->GetCollectionID("detectorHitsCollection");
    }

    if ( fRoomHCID == -1 ) {
        fRoomHCID
                = G4SDManager::GetSDMpointer()->GetCollectionID("RoomHitsCollection");
    }

    // Get hits collections
    auto detectorHC = GetHitsCollection(fdetectorHCID, event);
    auto roomHC = GetHitsCollection(fRoomHCID, event);//proveArgon


    // Get hit with total values
    auto detectorHit = (*detectorHC)[detectorHC->entries()-1];
    auto roomHit = (*roomHC)[roomHC->entries()-1];//proveArgon



    //Raccoglie le informazioni sull'evento primario
    //GetPrimaryVertex(i) prende le info sull'i-esimo vertice, 0 è il primo in assoluto, quindi quello dove viene generata la particella incidente

    G4PrimaryVertex* pv = event->GetPrimaryVertex(0);
    // pv->GetT0()/CLHEP::ns  //questo è per avere il tempo del vertice, lo tengo come reminder
    G4ThreeVector generatorPosition = G4ThreeVector(pv->GetX0(),pv->GetY0(),pv->GetZ0());

    //Qui ci sono ulteriori info sulla particella primaria tipo energia ed impulso che in questo momento non stai usando, ma se per esempio vuoi usare un fascio non parallelo puoi decommentare qui
    G4PrimaryParticle* pp = pv->GetPrimary();
    G4double generatorEnergy = pp->GetKineticEnergy() + pp->GetMass();
    G4double E0 = generatorEnergy;
    //G4ThreeVector generatorMomentum =  pp->GetMomentumDirection();
/*
    G4int nAr = 0;
    G4int nPrimaries = event->GetNumberOfPrimaryVertex();//proveArgon
    for (G4int i = 0; i < nPrimaries; ++i) {
        G4PrimaryVertex* vertex = event->GetPrimaryVertex(i);
        G4PrimaryParticle* primary = vertex->GetPrimary();

        G4Track* track = primary->GetPrimaryTrack();
        if (!track) continue;

        G4TrackVector* secondaries = track->GetSecondaries();
        if (!secondaries) continue;

        for (size_t j = 0; j < secondaries->size(); ++j) {
            G4Track* secondaryTrack = (*secondaries)[j];
            if (secondaryTrack->GetDefinition()->GetParticleName() == "Ar41") {
                ++nAr;
            }
        }
    }
    G4cout << "Number of Ar41 particles produced in this event: " << nAr << G4endl;
    */
    // Print per event (modulo n)
    //
    auto eventID = event->GetEventID();
    auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
    if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) { //checks if the printing frequency is greater than zero and if the current event ID is a multiple of the printing frequency. If both conditions are met, event statistics will be printed.
        G4cout << "---> End of event: " << eventID << G4endl;
        const long* seeds = CLHEP::HepRandom::getTheSeeds();
        G4cout << "Event: " << eventID << " Seeds: " << seeds[0] << ", " << seeds[1] << G4endl;

        PrintEventStatistics(
                detectorHit->GetNNeutrons(), detectorHit->GetENeutrons(),
                //detectorHit->GetEdep(),
                //roomHit->GetNAr41(), roomHit->GetEAr41(), //proveArgon
                ///roomHit->GetSecondaryEnergy(),  roomHit->GetAtomicMass(), roomHit->GetAtomicNumber(),
                detectorHit->GetXpos(), detectorHit->GetYpos(), generatorPosition);
    }

    // Fill histograms, ntuple
    //

    // get analysis manager
    auto analysisManager = G4AnalysisManager::Instance();

    //Qui salva le informazioni nel tree e negli istogrammi, la funzione GetNNeutrons() è definita nel CalorHit.hh e semplicemente legge il numero di fotoni salvato nella variabile fPhotons della collezione di hit.

    // fill histograms
    if(detectorHit->GetENeutrons()>=0 ){
        //if(detectorHit->GetXpos()>=-25 && detectorHit->GetYpos()>=-25 && detectorHit->GetXpos()<=25 && detectorHit->GetYpos()<=25){
            analysisManager->FillH1(0, E0);
       // }
    }

    if(roomHit->GetEAr41()>=0){
        analysisManager->FillH1(1, roomHit->GetEAr41());}

    if(roomHit->GetBoundaryEnergy()>=0) {
        analysisManager->FillH1(2, roomHit->GetBoundaryEnergy());
        analysisManager->FillH1(3, roomHit->GetBoundaryPosition().z());
        if(abs(roomHit->GetBoundaryPosition().z())<=1.51*CLHEP::m ) {
            analysisManager->FillH2(4, roomHit->GetBoundaryPosition().z(),roomHit->GetBoundaryEnergy());        }
    }

    std::vector<G4double> particleEnergies = roomHit->GetSecondaryEnergy();
    std::vector<G4int> atomicMasses = roomHit->GetAtomicMass();
    std::vector<G4int> atomicNumbers = roomHit->GetAtomicNumber();
    if (atomicMasses[0]!=-1) {
        for (size_t i = 0; i < atomicMasses.size(); ++i){
            if (atomicMasses[i] == -1) {
                break;
            }
            else {
                G4int A = atomicMasses[i];
                G4int Z = atomicNumbers[i];
                analysisManager->FillH2(0, A, Z);
                std::string fparticleID = "A=" + std::to_string(A) + " Z=" + std::to_string(Z);
                secondaryParticleCounts[fparticleID]++;
            }
            //G4cout << "atomic number and mass" <<A << Z << G4endl;
        }
    }

   if(detectorHit->GetXpos()>-888 && detectorHit->GetYpos()>-888){
        analysisManager->FillH2(1, detectorHit->GetXpos(), detectorHit->GetYpos());}

    //analysisManager->FillH2(2, generatorPosition[0], generatorPosition[1]);

    analysisManager->FillH2(2, generatorPosition[0], generatorPosition[1]);

    if(roomHit->GetBoundaryPosition().x()>=3./2*CLHEP::m ) {
        //analysisManager->FillH1(3, roomHit->GetBoundaryEnergy());
        analysisManager->FillH2(3, roomHit->GetBoundaryPosition().z(), roomHit->GetBoundaryPosition().y());
    }

    // fill ntuple
/*
    if (!particleEnergies.empty()) {
        for (size_t i = 0; i < atomicMasses.size(); ++i){
            G4int A= atomicMasses[i];
            G4int Z= atomicNumbers[i];
            analysisManager->FillH2(0,  A,  Z);
            analysisManager->FillNtupleDColumn(0,  A);
            analysisManager->FillNtupleDColumn(1,  Z); //nb: in questo modo se il vettore ha piu di un elemento ne salva counque solo uno---> to be fixed
            //G4cout << "atomic number and mass" <<A << Z << G4endl;
        }
    }
    else{
        analysisManager->FillNtupleDColumn(0,-1);
        analysisManager->FillNtupleDColumn(1,-1);
    }
*/
    //analysisManager->FillNtupleDColumn(0, gapHit->GetNNeutrons());
    //analysisManager->FillNtupleDColumn(0, gapHit->GetENeutrons());
    //analysisManager->FillNtupleDColumn(2, gapHit->GetEdep());
    //analysisManager->FillNtupleDColumn(3, roomHit->GetNAr41()); //proveArgon nb numeri cambiati
    //analysisManager->FillNtupleDColumn(1, roomHit->GetEAr41()); //proveArgon nb numeri cambiati

    //analysisManager->FillNtupleDColumn(2, gapHit->GetXpos());
    //analysisManager->FillNtupleDColumn(3, gapHit->GetYpos());
    //analysisManager->FillNtupleDColumn(10, generatorPosition[0]);
    //analysisManager->FillNtupleDColumn(11, generatorPosition[1]);
    //analysisManager->FillNtupleDColumn(0, roomHit->GetBoundaryPosition().x());
    //analysisManager->FillNtupleDColumn(1, roomHit->GetBoundaryPosition().y());
    //analysisManager->FillNtupleDColumn(2, roomHit->GetBoundaryPosition().z());
    //analysisManager->FillNtupleDColumn(3, roomHit->GetBoundaryEnergy());
    //analysisManager->AddNtupleRow();

}  
