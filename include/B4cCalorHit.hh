
#ifndef B4cCalorHit_h
#define B4cCalorHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4Threading.hh"

/// Calorimeter hit class
///
/// It defines data members to store the the energy deposit and track lengths
/// of charged particles in a selected volume:
/// - fEdep, fTrackLength


// ho cambiato Photons in Neutrons
class B4cCalorHit : public G4VHit
{
  public:
    B4cCalorHit();
    B4cCalorHit(const B4cCalorHit&);
    virtual ~B4cCalorHit();

    // operators
    const B4cCalorHit& operator=(const B4cCalorHit&);
    G4bool operator==(const B4cCalorHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    // methods from base class
    virtual void Draw() {}
    virtual void Print();

    // methods to handle data
    void Add(G4double de, G4double dl);
    void AddNeutron(G4double neutronE);
    void AddAr41(G4double Ar41E);
    //%%%%%%%%%%%%%%%%%%%%%% Secondary particles analysis %%%%%%%%%%%%%%%%%%%%%%%%%%
    void AddSecondaryParticle(G4double secondaryE, G4int A, G4int Z);
    /*
    void AddN15(G4double N15E);
    void AddO17(G4double O17E);
    void AddProton(G4double ProtonE);
    void AddGamma(G4double GammaE);
    void AddElectron(G4double ElectronE);
     */
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    void AddPosition(G4double posx, G4double posy);

    // get methods
    G4double GetEdep() const;
    G4double GetTrackLength() const;
    G4int GetNNeutrons() const;
    G4double GetENeutrons() const;
    G4int GetNAr41() const;
    G4double GetEAr41() const;
    //%%%%%%%%%%%%%%%%%%%%%% Secondary particles analysis %%%%%%%%%%%%%%%%%%%%%%%%%%
    G4int GetAtomicMass() const;
    G4int GetAtomicNumber() const;
    G4double GetSecondaryEnergy() const;
    /*
    G4int GetNN15() const;
    G4double GetEN15() const;
    G4int GetNO17() const;
    G4double GetEO17() const;
    G4int GetNProton() const;
    G4double GetEProton() const;
    G4int GetNGamma() const;
    G4double GetEGamma() const;
    G4int GetNElectron() const;
    G4double GetEElectron() const;
     */
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    G4double GetXpos() const;
    G4double GetYpos() const;


private:
    G4double fEdep;        ///< Energy deposit in the sensitive volume
    G4double fTrackLength; ///< Track length in the  sensitive volume
    G4double fNeutrons;
    G4double eNeutron;
    G4double fAr41;
    G4double eAr41;
    //%%%%%%%%%%%%%%%%%%%%%% Secondary particles analysis %%%%%%%%%%%%%%%%%%%%%%%%%%
    G4double fA;
    G4double fZ;
    G4double eSecondary;
    /*
    G4double fN15;
    G4double eN15;
    G4double fO17;
    G4double eO17;
    G4double fProton;
    G4double eProton;
    G4double fGamma;
    G4double eGamma;
    G4double fElectron;
    G4double eElectron;
     */
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    G4double xPos;
    G4double yPos;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

using B4cCalorHitsCollection = G4THitsCollection<B4cCalorHit>;

extern G4ThreadLocal G4Allocator<B4cCalorHit>* B4cCalorHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* B4cCalorHit::operator new(size_t)
{
  if (!B4cCalorHitAllocator) {
    B4cCalorHitAllocator = new G4Allocator<B4cCalorHit>;
  }
  void *hit;
  hit = (void *) B4cCalorHitAllocator->MallocSingle();
  return hit;
}

inline void B4cCalorHit::operator delete(void *hit)
{
  if (!B4cCalorHitAllocator) {
    B4cCalorHitAllocator = new G4Allocator<B4cCalorHit>;
  }
  B4cCalorHitAllocator->FreeSingle((B4cCalorHit*) hit);
}

inline void B4cCalorHit::Add(G4double de, G4double dl) {
  fEdep += de; 
  fTrackLength += dl;
}

inline G4double B4cCalorHit::GetEdep() const { 
  return fEdep; 
}

inline G4double B4cCalorHit::GetTrackLength() const { 
  return fTrackLength; 
}

inline G4int B4cCalorHit::GetNNeutrons() const {
  return int(fNeutrons);
}

inline G4double B4cCalorHit::GetENeutrons() const {
  return eNeutron;
}

inline G4int B4cCalorHit::GetNAr41() const { //proveArgon
  return int(fAr41);
}

inline G4double B4cCalorHit::GetEAr41() const { //proveArgon
    return eAr41;
}

//%%%%%%%%%%%%%%%%%%%%%% Secondary particles analysis %%%%%%%%%%%%%%%%%%%%%%%%%%
inline G4int B4cCalorHit::GetAtomicNumber() const { //proveArgon
    return int(fZ);
}

inline G4int B4cCalorHit::GetAtomicMass() const { //proveArgon
    return int(fA);
}

inline G4double B4cCalorHit::GetSecondaryEnergy() const { //proveArgon
    return double(eSecondary);
}

/*
inline G4int B4cCalorHit::GetNN15() const { //proveArgon
    return int(fN15);
}

inline G4double B4cCalorHit::GetEN15() const { //proveArgon
    return eN15;
}

inline G4int B4cCalorHit::GetNO17() const { //proveArgon
    return int(fO17);
}

inline G4double B4cCalorHit::GetEO17() const { //proveArgon
    return eO17;
}

inline G4int B4cCalorHit::GetNProton() const { //proveArgon
    return int(fProton);
}

inline G4double B4cCalorHit::GetEProton() const { //proveArgon
    return eProton;
}

inline G4int B4cCalorHit::GetNGamma() const { //proveArgon

    return int(fGamma);
}

inline G4double B4cCalorHit::GetEGamma() const { //proveArgon

    return eGamma;
}

inline G4int B4cCalorHit::GetNElectron() const { //proveArgon
    return int(fElectron);
}

inline G4double B4cCalorHit::GetEElectron() const { //proveArgon
    return eElectron;
}
*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


inline G4double B4cCalorHit::GetXpos() const {
  return xPos;
}

inline G4double B4cCalorHit::GetYpos() const {
  return yPos;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
