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
	
	G4int runID = run->GetRunID();
	std::stringstream strRunID;
	strRunID << runID;
	
	
	man->OpenFile("output"+strRunID.str()+".root");
	
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
