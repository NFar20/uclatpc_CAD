#include "run.hh"

MyRunAction::MyRunAction()
{
	G4AnalysisManager *man = G4AnalysisManager::Instance();
	
	man->CreateNtuple("Hits", "Hits");
	man->CreateNtupleIColumn("fEvent");
	man->CreateNtupleIColumn("fType");
	man->CreateNtupleIColumn("fParentID");
	man->CreateNtupleDColumn("fX");
	man->CreateNtupleDColumn("fY");
	man->CreateNtupleDColumn("fZ");
	man->FinishNtuple(0);
	
	man->CreateNtuple("Edep", "Edep");
	man->CreateNtupleDColumn("fEdep");
	man->FinishNtuple(1);
}

MyRunAction::~MyRunAction()
{}

void MyRunAction::BeginOfRunAction(const G4Run* run)
{
	//make data file here instead of in the constructor, so a new file is created for every run event
	G4AnalysisManager *man = G4AnalysisManager::Instance();

	man->SetVerboseLevel(1);
	
	G4int runID = run->GetRunID();
	std::stringstream strRunID;
	strRunID << runID;
	
	man->OpenFile("scintillation"+strRunID.str()+".root");
	const G4int bins = 2500;
	const G4double tmin = 0.*ns;
	const G4double tmax = 50000*ns;

	man->CreateH1("hAllTime", "All photon times;time [#mu s];hits", bins, tmin, tmax);
  	man->CreateH1("hS1Time",  "S1 photon times;time [#mu s];hits",  bins, tmin, tmax);
  	man->CreateH1("hS2Time",  "S2 photon times;time [#mu s];hits",  bins, tmin, tmax);

	
	// man->OpenFile("output"+strRunID.str()+".root");
	
}

void MyRunAction::EndOfRunAction(const G4Run*)
{
	G4cout << "Number of S1 events: " << nS1Events << G4endl;
	G4cout << "Number of S2 events: " << nS2Events << G4endl;

    G4cout << "Total S1 Photons: " << totalS1Photons << G4endl;
	G4cout << "Total S2 Photons: " << totalS2Photons << G4endl;
	
	G4AnalysisManager *man = G4AnalysisManager::Instance();
	
	man->Write();
	man->CloseFile();
}
