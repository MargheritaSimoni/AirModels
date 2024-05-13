#ifndef B4cEventAction_h
#define B4cEventAction_h 1

#include "G4UserEventAction.hh"

#include "B4cCalorHit.hh"

#include "globals.hh"

/// Event action class
///
/// In EndOfEventAction(), it prints the accumulated quantities of the energy 
/// deposit and track lengths of charged particles in Absober and Gap layers 
/// stored in the hits collections.

class B4cEventAction : public G4UserEventAction
{
public:
  B4cEventAction();
  virtual ~B4cEventAction();

  virtual void  BeginOfEventAction(const G4Event* event);
  virtual void    EndOfEventAction(const G4Event* event);
    
private:
  // methods
  B4cCalorHitsCollection* GetHitsCollection(G4int hcID,
                                            const G4Event* event) const;
  void PrintEventStatistics(G4double nNeutrons, G4double eNeutrons,
                            G4double Edep,
                           // G4double nAr41, G4double eAr41,
          //%%%%%%%%%%%%%%%%%%%%%% Secondary particles analysis %%%%%%%%%%%%%%%%%%%%%%%%%%
                            G4double eSec, G4int nA, G4int nZ,

          /*
                            G4double nN15, G4double eN15,
                            G4double nO17, G4double eO17,
                            G4double nProton, G4double eProton,
                            G4double nGamma, G4double eGamma,
                            G4double nElectron, G4double eElectron,
          */
          //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                            G4double xPos, G4double yPos, G4ThreeVector gp) const; //proveArgon va modificato??

  // data members
  G4int  fAbsHCID;
  G4int  fGapHCID;
  G4int  fRoomHCID;//proveArgon
};
                     
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
