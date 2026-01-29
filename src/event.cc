#include "event.hh"

extern int totalS1Photons;
extern int totalS2Photons;
extern int incidentRecoilType;

MyEventAction::MyEventAction(MyRunAction*)
{
	fEdep = 0.;
}

MyEventAction::~MyEventAction()
{}

void MyEventAction::BeginOfEventAction(const G4Event* evt)
{
	fEdep = 0.;
    fEventID = evt->GetEventID();
    fStepCounter = 0;
}

void MyEventAction::EndOfEventAction(const G4Event*)
{
	G4cout << "Energy deposition: " << fEdep << G4endl;
	
	G4AnalysisManager *man = G4AnalysisManager::Instance();
	
	man->FillNtupleDColumn(1, 0, fEdep);
	
	man->AddNtupleRow(1);
	if(totalS2Photons > 0) // avoid log10(0)
    {  
        man->FillNtupleDColumn(4, 0, std::log10(totalS2Photons));
    } 
    else 
    {
        man->FillNtupleDColumn(4, 0, -10);
    }
    man->FillNtupleDColumn(4, 1, totalS1Photons);
    man->FillNtupleIColumn(4, 2, incidentRecoilType);
	man->AddNtupleRow(4);
}

void MyEventAction::AddEdep(G4double edep)
{
    fEdep += edep;
}

void MyEventAction::AddData(G4double edep_keV, G4double x_mm, G4double y_mm, G4double z_mm,
    G4double time,
    G4double Efield_Vcm,
    G4int type, G4int trackID, G4int parentID, G4int pdg,
    const G4String& process,
    const G4String& volume,
    const G4String& material)
{
    G4AnalysisManager *man = G4AnalysisManager::Instance();
    const G4int stepID = fStepCounter++;

    man->FillNtupleIColumn(4, 0, fEventID);
    man->FillNtupleIColumn(4, 1, stepID);
    man->FillNtupleDColumn(4, 2, edep_keV);
    man->FillNtupleDColumn(4, 3, x_mm);
    man->FillNtupleDColumn(4, 4, y_mm);
    man->FillNtupleDColumn(4, 5, z_mm);
    man->FillNtupleDColumn(4, 6, time);
    man->FillNtupleDColumn(4, 7, Efield_Vcm);
    man->FillNtupleIColumn(4, 8, type);
    man->FillNtupleIColumn(4, 9, trackID);
    man->FillNtupleIColumn(4, 10, parentID);
    man->FillNtupleIColumn(4, 11, pdg);

    man->FillNtupleSColumn(4, 12, process);
    man->FillNtupleSColumn(4, 13, volume);
    man->FillNtupleSColumn(4, 14, material);

    man->AddNtupleRow();

}

