#include "B4cCalorHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include <iomanip>

// nb: scambiato tutti gli fPhotons con fNeutrons

G4ThreadLocal G4Allocator<B4cCalorHit>* B4cCalorHitAllocator = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cCalorHit::B4cCalorHit()
        : G4VHit(),
          fEdep(0.),
          fTrackLength(0.),
          fNeutrons(0),
          eNeutron(-999),
          fAr41(0),
          eAr41(-999),
          ArPos(-1000,-1000,-1000),
          fA(5000,-1),
          fZ(5000,-1),
          eSecondary(5000,0),
          secondaryTrackLengths(5000,0),
          boundaryVector(-1000,-1000,-1000),
          boundaryEnergy(-999),
          xPos(-888),
          yPos(-888)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cCalorHit::~B4cCalorHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cCalorHit::B4cCalorHit(const B4cCalorHit& right)
        : G4VHit()
{
    fEdep = right.fEdep;
    fTrackLength = right.fTrackLength;

    fNeutrons = right.fNeutrons;
    eNeutron = right.eNeutron;

    fAr41 = right.fAr41;
    eAr41 = right.eAr41;
    ArPos = right.ArPos;


    fA = right.fA;
    fZ = right.fZ;
    eSecondary = right.eSecondary;
    secondaryTrackLengths = right.secondaryTrackLengths;

    boundaryVector = right.boundaryVector;
    boundaryEnergy = right.boundaryEnergy;

    xPos = right.xPos;
    yPos = right.yPos;

    // New logic: copy counted TrackIDs and energy/position vectors
    fCountedAr41TrackIDs = right.fCountedAr41TrackIDs;

    eAr41Vector = right.eAr41Vector;

    Ar41PosVector = right.Ar41PosVector;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const B4cCalorHit& B4cCalorHit::operator=(const B4cCalorHit& right)
{
    if (this == &right)
        return *this;

    fEdep = right.fEdep;
    fTrackLength = right.fTrackLength;

    fNeutrons = right.fNeutrons;
    eNeutron = right.eNeutron;

    fAr41 = right.fAr41;
    eAr41 = right.eAr41;
    ArPos = right.ArPos;

    fA = right.fA;
    fZ = right.fZ;
    eSecondary = right.eSecondary;
    secondaryTrackLengths = right.secondaryTrackLengths;

    boundaryVector = right.boundaryVector;
    boundaryEnergy = right.boundaryEnergy;

    xPos = right.xPos;
    yPos = right.yPos;

    // New logic: copy counted TrackIDs and energy/position vectors
    fCountedAr41TrackIDs = right.fCountedAr41TrackIDs;

    eAr41Vector = right.eAr41Vector;

    Ar41PosVector = right.Ar41PosVector;

    return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool B4cCalorHit::operator==(const B4cCalorHit& right) const
{
    return (this == &right) ? true : false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cCalorHit::Print()
{
    G4cout
            << "Edep: "
            << std::setw(7) << G4BestUnit(fEdep, "Energy")
            << " track length: "
            << std::setw(7) << G4BestUnit(fTrackLength, "Length")
            << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cCalorHit::AddNeutronInDetector(G4double neutronE)
{
    fNeutrons += 1;
    eNeutron = neutronE;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cCalorHit::AddAr41(G4int trackID, G4double Ar41E, G4ThreeVector Ar41Position)
{
    // If this Ar41 track was already counted, do nothing
    if (fCountedAr41TrackIDs.find(trackID) != fCountedAr41TrackIDs.end())
    {
        return;
    }

    // Remember this Ar41 track
    fCountedAr41TrackIDs.insert(trackID);

    // Count this Ar41 only once
    fAr41 += 1;

    // Save the energy of this specific Ar41 particle
    eAr41Vector.push_back(Ar41E);

    // Save the position of this specific Ar41 particle
    Ar41PosVector.push_back(Ar41Position);

    // Keep also the old single-value variables updated
    // These will contain the last counted Ar41 particle
    eAr41 = Ar41E;
    ArPos = Ar41Position;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



//%%%%%%%%%%%%%%%%%%%%%% Secondary particles analysis %%%%%%%%%%%%%%%%%%%%%%%%%%

void B4cCalorHit::AddSecondaryParticle(
        G4int trackid,
        G4int Z,
        G4int A,
        G4double secondaryE,
        G4double trackLengths)
{
    fZ.at(trackid - 2) = Z;
    fA.at(trackid - 2) = A;
    eSecondary.at(trackid - 2) += secondaryE;
    secondaryTrackLengths.at(trackid - 2) = trackLengths;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cCalorHit::AddPositionInDetector(G4double posx, G4double posy)
{
    xPos = posx;
    yPos = posy;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cCalorHit::AddBoundaryTracking(G4ThreeVector posOnBoundary, G4double EOnBoundary)
{
    boundaryVector = posOnBoundary;
    boundaryEnergy = EOnBoundary;
}