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

// NOTA IMPORTANTE: abso qui è la collezione di hit nel PMT,
// xtal nel cristallo come puoi vedere a riga 79

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cEventAction::B4cEventAction()
        : G4UserEventAction(),
          fdetectorHCID(-1),
          fRoomHCID(-1)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cEventAction::~B4cEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cCalorHitsCollection*
B4cEventAction::GetHitsCollection(G4int hcID,
                                  const G4Event* event) const
{
    auto hitsCollection =
            static_cast<B4cCalorHitsCollection*>(
                    event->GetHCofThisEvent()->GetHC(hcID));

    if (!hitsCollection)
    {
        G4ExceptionDescription msg;
        msg << "Cannot access hitsCollection ID " << hcID;

        G4Exception("B4cEventAction::GetHitsCollection()",
                    "MyCode0003",
                    FatalException,
                    msg);
    }

    return hitsCollection;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cEventAction::PrintEventStatistics(
        G4double nNeutrons,
        G4double eNeutrons,
        G4double xPos,
        G4double yPos,
        G4ThreeVector gp) const
{
    G4cout
            << "   Neutrons in the detector: "
            << std::setw(7) << nNeutrons
            << G4endl
            << "       neutron energy: "
            << std::setw(7) << G4BestUnit(eNeutrons, "Energy")
            << G4endl
            << "        X position: "
            << std::setw(7) << G4BestUnit(xPos, "Length")
            << G4endl
            << "        Y position: "
            << std::setw(7) << G4BestUnit(yPos, "Length")
            << G4endl
            << "Generator Position: "
            << gp[0] << " " << gp[1] << " " << gp[2]
            << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cEventAction::BeginOfEventAction(const G4Event* /*event*/)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cEventAction::EndOfEventAction(const G4Event* event)
{
    // Get hits collections IDs only once
    if (fdetectorHCID == -1)
    {
        fdetectorHCID =
                G4SDManager::GetSDMpointer()
                        ->GetCollectionID("detectorHitsCollection");
    }

    if (fRoomHCID == -1)
    {
        fRoomHCID =
                G4SDManager::GetSDMpointer()
                        ->GetCollectionID("RoomHitsCollection");
    }

    // Get hits collections
    auto detectorHC = GetHitsCollection(fdetectorHCID, event);
    auto roomHC = GetHitsCollection(fRoomHCID, event);

    // Get hit with total values
    auto detectorHit = (*detectorHC)[detectorHC->entries() - 1];
    auto roomHit = (*roomHC)[roomHC->entries() - 1];

    // Primary generator information
    G4PrimaryVertex* pv = event->GetPrimaryVertex(0);

    G4ThreeVector generatorPosition =
            G4ThreeVector(pv->GetX0(), pv->GetY0(), pv->GetZ0());

    G4PrimaryParticle* pp = pv->GetPrimary();

    G4double generatorEnergy =
            pp->GetKineticEnergy() + pp->GetMass();

    G4double E0 = generatorEnergy;

    // Print per event
    auto eventID = event->GetEventID();
    auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();

    if ((printModulo > 0) && (eventID % printModulo == 0))
    {
        G4cout << "---> End of event: " << eventID << G4endl;

        const long* seeds = CLHEP::HepRandom::getTheSeeds();

        G4cout << "Event: " << eventID
               << " Seeds: " << seeds[0]
               << ", " << seeds[1]
               << G4endl;

        PrintEventStatistics(
                detectorHit->GetNNeutrons(),
                detectorHit->GetENeutrons(),
                detectorHit->GetXpos(),
                detectorHit->GetYpos(),
                generatorPosition
        );
    }

    // Get analysis manager
    auto analysisManager = G4AnalysisManager::Instance();

    /////////////////////////////////////////////////////////
    // Neutrons in detector
    /////////////////////////////////////////////////////////

    if (detectorHit->GetENeutrons() >= 0)
    {
        analysisManager->FillH1(0, E0);
    }

    /////////////////////////////////////////////////////////
    // Ar41 energies
    /////////////////////////////////////////////////////////

    std::vector<G4double> ar41Energies =
            roomHit->GetEAr41Vector();

    for (size_t i = 0; i < ar41Energies.size(); ++i)
    {
        analysisManager->FillH1(1, ar41Energies[i]);
    }


    /////////////////////////////////////////////////////////
    // Boundary neutron information
    /////////////////////////////////////////////////////////

    if (roomHit->GetBoundaryEnergy() >= 0)
    {
        analysisManager->FillH1(2, roomHit->GetBoundaryEnergy());
        analysisManager->FillH1(3, roomHit->GetBoundaryPosition().z());

        if (std::abs(roomHit->GetBoundaryPosition().z()) <= 1.51 * CLHEP::m)
        {
            analysisManager->FillH2(
                    4,
                    roomHit->GetBoundaryPosition().z(),
                    roomHit->GetBoundaryEnergy()
            );
        }
    }

    /////////////////////////////////////////////////////////
    // Secondary particle A/Z counting
    /////////////////////////////////////////////////////////

    std::vector<G4double> particleEnergies =
            roomHit->GetSecondaryEnergy();

    std::vector<G4int> atomicMasses =
            roomHit->GetAtomicMass();

    std::vector<G4int> atomicNumbers =
            roomHit->GetAtomicNumber();

    for (size_t i = 0; i < atomicMasses.size(); ++i)
    {
        if (atomicMasses[i] == -1)
            continue;

        G4int A = atomicMasses[i];
        G4int Z = atomicNumbers[i];

        analysisManager->FillH2(0, A, Z);

        std::string fparticleID =
                "A=" + std::to_string(A) + " Z=" + std::to_string(Z);

        secondaryParticleCounts[fparticleID]++;
    }

    /////////////////////////////////////////////////////////
    // Detector position
    /////////////////////////////////////////////////////////

    if (detectorHit->GetXpos() > -888 &&
        detectorHit->GetYpos() > -888)
    {
        analysisManager->FillH2(
                1,
                detectorHit->GetXpos(),
                detectorHit->GetYpos()
        );
    }

    /////////////////////////////////////////////////////////
    // Generator position
    /////////////////////////////////////////////////////////

    analysisManager->FillH2(
            2,
            generatorPosition[0],
            generatorPosition[1]
    );

    /////////////////////////////////////////////////////////
    // Side position
    /////////////////////////////////////////////////////////

    if (roomHit->GetBoundaryPosition().x() >= 3. / 2 * CLHEP::m)
    {
        analysisManager->FillH2(
                3,
                roomHit->GetBoundaryPosition().z(),
                roomHit->GetBoundaryPosition().y()
        );
    }

    /////////////////////////////////////////////////////////
    // Ntuple currently disabled
    /////////////////////////////////////////////////////////
}