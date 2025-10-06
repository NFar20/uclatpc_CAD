#include "event.hh"

MyEventAction::MyEventAction()
{
	fEdep = 0.;
}

MyEventAction::~MyEventAction()
{}

void MyEventAction::BeginOfEventAction(const G4Event*)
{
	fEdep = 0.;
}

void MyEventAction::EndOfEventAction(const G4Event*)
{
	G4cout << "Energy deposition: " << fEdep << G4endl;
	
	G4AnalysisManager *man = G4AnalysisManager::Instance();
	
	// man->FillNtupleDColumn(1, 0, fEdep);
	
	// man->AddNtupleRow(1);
	if(totalS2Photons > 0) // avoid log10(0)
    {  
        man->FillNtupleDColumn(1, 0, std::log10(totalS2Photons));
    } 
    else 
    {
        man->FillNtupleDColumn(1, 0, -10);
    }
    man->FillNtupleDColumn(1, 1, totalS1Photons);
    man->FillNtupleIColumn(1, 2, incidentRecoilType);
	man->AddNtupleRow(1);
}

