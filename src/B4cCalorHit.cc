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
   //%%%%%%%%%%%%%%%%%%%%%% Secondary particles analysis %%%%%%%%%%%%%%%%%%%%%%%%%%
   fA(-1),
   fZ(-1),
   eSecondary(-999),
   /*
   fN15(0),
   eN15(-999),
   fO17(0),
   eO17(-999),
   fProton(0),
   eProton(-999),
   fGamma(0),
   eGamma(-1),
   fElectron(0),
   eElectron(-999),
    */
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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
    //%%%%%%%%%%%%%%%%%%%%%% Secondary particles analysis %%%%%%%%%%%%%%%%%%%%%%%%%%
    fA = right.fA;
    fZ = right.fZ;
    eSecondary = right.eSecondary;
    /*
    fN15 = right.fN15;
    eN15 = right.eN15;
    fO17 = right.fO17;
    eO17 = right.eO17;
    fProton = right.fProton;
    eProton = right.eProton;
    fGamma = right.fGamma;
    eGamma = right.eGamma;
    fElectron = right.fElectron;
    eElectron = right.eElectron;
     */
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    xPos = right.xPos;
    yPos = right.yPos;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const B4cCalorHit& B4cCalorHit::operator=(const B4cCalorHit& right)
{
    fEdep = right.fEdep;
    fTrackLength = right.fTrackLength;
    fNeutrons = right.fNeutrons;
    eNeutron = right.eNeutron;
    fAr41 = right.fAr41;
    eAr41 = right.eAr41;
    //%%%%%%%%%%%%%%%%%%%%%% Secondary particles analysis %%%%%%%%%%%%%%%%%%%%%%%%%%
    fA = right.fA;
    fZ = right.fZ;
    eSecondary = right.eSecondary;
    /*
    fN15 = right.fN15;
    eN15 = right.eN15;
    fO17 = right.fO17;
    eO17 = right.eO17;
    fProton = right.fProton;
    eProton = right.eProton;
    fGamma = right.fGamma;
    eGamma = right.eGamma;
    fElectron = right.fElectron;
    eElectron = right.eElectron;
     */
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    xPos = right.xPos;
    yPos = right.yPos;

  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool B4cCalorHit::operator==(const B4cCalorHit& right) const
{
  return ( this == &right ) ? true : false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cCalorHit::Print()
{
  G4cout
     << "Edep: " 
     << std::setw(7) << G4BestUnit(fEdep,"Energy")
     << " track length: " 
     << std::setw(7) << G4BestUnit( fTrackLength,"Length")
     << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cCalorHit::AddNeutron(G4double neutronE) {
        fNeutrons += 1;
        eNeutron = neutronE;
   };

void B4cCalorHit::AddAr41(G4double Ar41E) {
    fAr41 += 1;
    eAr41 = Ar41E;
};

//%%%%%%%%%%%%%%%%%%%%%% Secondary particles analysis %%%%%%%%%%%%%%%%%%%%%%%%%%
void B4cCalorHit::AddSecondaryParticle(G4double secondaryE, G4int A, G4int Z) {
    fA = A;
    fZ = Z;
    eSecondary = secondaryE;
};
/*
void B4cCalorHit::AddN15(G4double N15E) {
    fN15 += 1;
    eN15 = N15E;
};
void B4cCalorHit::AddO17(G4double O17E) {
    fO17 += 1;
    eO17 = O17E;
};
void B4cCalorHit::AddProton(G4double ProtonE) {
    fProton += 1;
    eProton = ProtonE;
};
void B4cCalorHit::AddGamma(G4double GammaE) {
    fGamma += 1;
    eGamma = GammaE;
};
void B4cCalorHit::AddElectron(G4double ElectronE) {
    fElectron += 1;
    eElectron = ElectronE;
};
 */
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void B4cCalorHit::AddPosition(G4double posx, G4double posy) {
    xPos = posx;
    yPos = posy;
};
