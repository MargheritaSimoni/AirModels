#include "B4RunAction.hh"
#include "B4Analysis.hh"
#include "B4cEventAction.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4RunAction::B4RunAction()
: G4UserRunAction()
{ 
    // set printing event number per each event
    G4RunManager::GetRunManager()->SetPrintProgress(1);
    
    // Create analysis manager
    // The choice of analysis technology is done via selectin of a namespace
    // in B4Analysis.hh
    auto analysisManager = G4AnalysisManager::Instance();
    G4cout << "Using " << analysisManager->GetType() << G4endl;
    
    // Create directories
    //analysisManager->SetHistoDirectoryName("histograms");
    //analysisManager->SetNtupleDirectoryName("ntuple");
    analysisManager->SetVerboseLevel(2);
    analysisManager->SetNtupleMerging(false);
    // Note: merging ntuples is available only with Root output
    
    // Book histograms, ntuple
    //
    
    // Creating histograms // name and description of istograms
    analysisManager->CreateH1("ENeutron","Neutron Energy in Detector",300, 0.005e-04*eV, 0.025*eV,"eV","none","log"); // third entry is type of binning (log binning)//nb min can't be 0 in log scale//nb da anche il max dell'istogramma
    analysisManager->SetH1XAxisTitle(0, "Energy [eV]");
    analysisManager->SetH1YAxisTitle(0, "Counts");

    analysisManager->CreateH1("EAr41","Energy of Ar-41",100, 0.01*eV, 1*GeV, "MeV","none","log"); // H1= 1D istogram //proveArgon
    analysisManager->SetH1XAxisTitle(1, "Energy [MeV]");
    analysisManager->SetH1YAxisTitle(1, "Counts");

    analysisManager->CreateH1("EBoundary","Energy on boundary of solid ",300, 0.005e-04*eV, 0.025*eV, "eV","none","log"); // third entry is type of binning (log binning)//nb min can't be 0 in log scale//nb da anche il max dell'istogramma
    analysisManager->SetH1XAxisTitle(2, "Energy [eV]");
    analysisManager->SetH1YAxisTitle(2, "Counts");

    analysisManager->CreateH1("ZBoundary","Z coordinate on boundary of solid ",299, -1.50*m, 1.50*m, "m"); // third entry is type of binning (log binning)//nb min can't be 0 in log scale//nb da anche il max dell'istogramma
    analysisManager->SetH1XAxisTitle(3, "z [m]");
    analysisManager->SetH1YAxisTitle(3, "Counts");

    analysisManager->CreateH2("AZ","Atomic number and atomic mass", 50, -0.5, 50.5, 50, -0.5, 20.5);
    analysisManager->SetH2XAxisTitle(0, "A");
    analysisManager->SetH2YAxisTitle(0, "Z");

    analysisManager->CreateH2("DetPos","Position in detector", 200, -2.5*cm, 2.5*cm, 200, -2.5*cm, 2.5*cm, "cm", "cm");
    analysisManager->SetH2XAxisTitle(1, "x [cm]");
    analysisManager->SetH2YAxisTitle(1, "y [cm]");

    analysisManager->CreateH2("GenPos","Generator Position", 200, -2.5*cm, 2.5*cm, 200, -2.5*cm, 2.5*cm, "cm", "cm");
    analysisManager->SetH2XAxisTitle(2, "x [cm]");
    analysisManager->SetH2YAxisTitle(2, "y [cm]");

    analysisManager->CreateH2("SidePos","position on one side of solid", 99, -1.50*m, 1.50*m, 99, -1.50*m, 1.50*m, "m", "m");
    analysisManager->SetH2XAxisTitle(3, "z [m]");
    analysisManager->SetH2YAxisTitle(3, "y [m]");

    analysisManager->CreateH2("EBvsZB","Energy vs Z on boundary", 99, -1.50*m, 1.50*m, 100, 0.005e-04*eV, 0.025*eV, "m", "eV", "none", "none", "linear", "linear");
    analysisManager->SetH2XAxisTitle(4, "Z [m]");
    analysisManager->SetH2YAxisTitle(4, "E [eV]");

    // Creating ntuple
    //
    analysisManager->CreateNtuple("B4", "Data Tree");
    //analysisManager->CreateNtupleDColumn("Neutron_num");
    //analysisManager->CreateNtupleDColumn("Neutron_ene");
    //analysisManager->CreateNtupleDColumn("Edep");
    //analysisManager->CreateNtupleDColumn("NAr41"); //proveArgon
    //analysisManager->CreateNtupleDColumn("EAr41"); //proveArgon
    // analysisManager->CreateNtupleDColumn("secondaryEnergy"); //proveArgon
    //analysisManager->CreateNtupleDColumn("A"); //proveArgon
    //analysisManager->CreateNtupleDColumn("Z"); //proveArgon
    //analysisManager->CreateNtupleDColumn("X");
    //analysisManager->CreateNtupleDColumn("Y");
    //analysisManager->CreateNtupleDColumn("X0");
    //analysisManager->CreateNtupleDColumn("Y0");
    analysisManager->CreateNtupleDColumn("XB"); //proveArgon
    analysisManager->CreateNtupleDColumn("YB"); //proveArgon
    analysisManager->CreateNtupleDColumn("ZB"); //proveArgon
    analysisManager->CreateNtupleDColumn("EB"); //proveArgon
    analysisManager->FinishNtuple();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4RunAction::~B4RunAction()
{
    delete G4AnalysisManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4RunAction::BeginOfRunAction(const G4Run* /*run*/)
{ 
    //inform the runManager to save random number seed
    //G4RunManager::GetRunManager()->SetRandomNumberStore(true);
    
    // Get analysis manager
    auto analysisManager = G4AnalysisManager::Instance();
    
    // Open an output file
    //
    G4String fileName = "B4";
    analysisManager->OpenFile(fileName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4RunAction::EndOfRunAction(const G4Run* /*run*/)
{
    // print histogram statistics
    //
    auto analysisManager = G4AnalysisManager::Instance();
    if ( analysisManager->GetH1(1) ) {
        G4cout << G4endl << " ----> print histograms statistic ";
        if(isMaster) {
            G4cout << "for the entire run " << G4endl << G4endl;
        }
        else {
            G4cout << "for the local thread " << G4endl << G4endl;
        }


        G4cout << " " << G4endl;
        G4cout << " Number of neutrons in transmission detector= "
               << analysisManager->GetH1(0)->entries() << G4endl;
        G4cout << " their average energy is = "
               << analysisManager->GetH1(0)->mean() << " eV "
               << ", their root mean square is= "
               << analysisManager->GetH1(0)->rms() << " eV" << G4endl;

        G4cout << " " << G4endl;
        G4cout << " Number of Argon 41 produced in the room is = "
               << analysisManager->GetH1(1)->entries() << G4endl;
        G4cout << " their average energy is = "
               << G4BestUnit(analysisManager->GetH1(1)->mean(), "Energy")
               << ", their root mean square is= "
               << G4BestUnit(analysisManager->GetH1(1)->rms(),  "Energy") << G4endl;
        G4cout << " " << G4endl;

        G4cout << " " << G4endl;
        G4cout << " Number of neutrons that come out of solid= "
               << analysisManager->GetH1(2)->entries() << G4endl;
        G4cout << " their average energy is = "
               << analysisManager->GetH1(2)->mean() << " eV "
               << ", their root mean square is= "
               << analysisManager->GetH1(2)->rms() << " eV" << G4endl;
        G4cout << " " << G4endl;

        G4cout << " Number of secondary particles produced "
               << analysisManager->GetH2(0)->entries() << G4endl;
        G4cout << " " << G4endl;


        // Get the event action
        const B4cEventAction* eventAction = static_cast<const B4cEventAction*>(G4RunManager::GetRunManager()->GetUserEventAction());
        // Get the secondary particle counts
        G4cout << "________________________________________________________________________________________________________" << G4endl;
        const std::map<std::string, int>& secondaryParticleCounts = eventAction->GetSecondaryParticleCounts();
        for (const auto& pair : secondaryParticleCounts) {
            G4cout << "SecondaryParticle " << pair.first << ", Counts " << pair.second << G4endl;
            G4cout << " " << G4endl;
        }
        G4cout << "N.b.: excited states and decay products are registered as different particles: Ar41->K41[1293.609]->K41 " << G4endl;
        G4cout << "________________________________________________________________________________________________________" << G4endl;
    }
    
    // save histograms & ntuple
    //
    analysisManager->Write();
    analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
