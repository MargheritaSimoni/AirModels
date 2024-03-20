#include "B4RunAction.hh"
#include "B4Analysis.hh"

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
    analysisManager->SetNtupleMerging(true);
    // Note: merging ntuples is available only with Root output
    
    // Book histograms, ntuple
    //
    
    // Creating histograms // name and description of istograms
    analysisManager->CreateH1("NNeutron","Neutron Number in Detector", 100, -1, 10);// numero di neutroni rivelati per ogni neutrone sparato
    analysisManager->CreateH1("ENeutron","Neutron Energy in Detector", 100, 0., 10*eV); //nb da anche il max dell'istogramma
    analysisManager->CreateH1("Egap","Edep in gap", 100, 0., 10*eV);
    //analysisManager->CreateH1("NAr41","Number of Ar-41", 100,-1,100); // H1= 1D istogram //proveArgon

    analysisManager->CreateH2("DetPos","Position in detector", 200, -2.5*m, 2.5*m, 200, -2.5*m, 2.5*m);
    analysisManager->SetH2XAxisTitle(0, "x [m]");
    analysisManager->SetH2YAxisTitle(0, "y [m]");

    analysisManager->CreateH2("GenPos","Generator Position", 200, -2.5*m, 2.5*m, 200, -2.5*m, 2.5*m);
    analysisManager->SetH2XAxisTitle(1, "x [m]");
    analysisManager->SetH2YAxisTitle(1, "y [m]");


    // Creating ntuple
    //
    analysisManager->CreateNtuple("B4", "Data Tree");
    analysisManager->CreateNtupleDColumn("Neutron_num");
    analysisManager->CreateNtupleDColumn("Neutron_ene");
    analysisManager->CreateNtupleDColumn("Edep");
    //analysisManager->CreateNtupleDColumn("Ar41"); //proveArgon

    analysisManager->CreateNtupleDColumn("X");
    analysisManager->CreateNtupleDColumn("Y");
    analysisManager->CreateNtupleDColumn("X0");
    analysisManager->CreateNtupleDColumn("Y0");
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
        
        G4cout << " NNeutrons : mean = "
        << analysisManager->GetH1(0)->mean()
        << " rms = "
        << analysisManager->GetH1(0)->rms() << G4endl;
        
        G4cout << " ENeutrons : mean = "
        << G4BestUnit(analysisManager->GetH1(1)->mean(), "Energy") 
        << " rms = "
        << G4BestUnit(analysisManager->GetH1(1)->rms(),  "Energy") << G4endl;
        
        G4cout << " Edep : mean = "
        << G4BestUnit(analysisManager->GetH1(2)->mean(), "Energy")
        << " rms = "
        << G4BestUnit(analysisManager->GetH1(2)->rms(),  "Energy") << G4endl;
        
        G4cout << " Xpos : mean = "
        << G4BestUnit(analysisManager->GetH2(0)->mean_x(), "Length")
        << " rms = "
        << G4BestUnit(analysisManager->GetH2(0)->rms_x(),  "Length") << G4endl;
        
        G4cout << " Ypos : mean = "
        << G4BestUnit(analysisManager->GetH2(0)->mean_y(), "Length")
        << " rms = "
        << G4BestUnit(analysisManager->GetH2(0)->rms_y(),  "Length") << G4endl;
    }
    
    // save histograms & ntuple
    //
    analysisManager->Write();
    analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
