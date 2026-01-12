#include "run.hh"

extern int nS1Events;
extern int nS2Events;
extern int totalS1Photons;
extern int totalS2Photons;
extern int yieldPhotons;
extern int yieldElectrons;
extern int check;

// std::ofstream MyRunAction::dataFile;

MyRunAction::MyRunAction()
{
	// dataFile.open("outputData.txt");

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

	man->CreateNtuple("tS2-tS1", "global_electron_drift_time");
	man->CreateNtupleDColumn("globalTime_us");
	man->CreateNtupleIColumn("trackID");
	man->FinishNtuple(2);

	man->CreateNtuple("drift", "electron_drift_time");
	man->CreateNtupleDColumn("driftTime_us");
	man->CreateNtupleIColumn("distance_mm");
	man->FinishNtuple(3);

	// const G4int bins = 1000; //2500

    // const double tmin = -5.; //microseconds 
	// const double tmax = 20; //microsends 
    // const double posMin = -35.; //mm
    // const double posMax = 35.; //mm

    // man->CreateH1("posX", " ", bins, posMin, posMax); 
    // man->CreateH1("posY", " ", bins, posMin, posMax); 
    // man->CreateH1("posZ", " ", bins, -30, 30); 
    // man->CreateH1("Time", " ", bins, tmin, tmax); 



}

MyRunAction::~MyRunAction()
{
	// dataFile.close();
}

void MyRunAction::BeginOfRunAction(const G4Run* run)
{
	//make data file here instead of in the constructor, so a new file is created for every run event

    DriftTrackIDs.clear();

	G4AnalysisManager *man = G4AnalysisManager::Instance();
	
	man->SetVerboseLevel(1);
	
	G4int runID = run->GetRunID();
	std::stringstream strRunID;
	strRunID << runID;

	if (!man->IsOpenFile()) {
    	man->OpenFile("output.root");
  	}
	
	// man->OpenFile("driftVelo"+strRunID.str()+".root");
	// const G4int bins = 2500;
	// const G4double tmin = 0.*ns;
	// const G4double tmax = 50000*ns;

	// man->CreateH1("hAllTime", "All photon times;time [#mu s];hits", bins, tmin, tmax);
  	// man->CreateH1("hS1Time",  "S1 photon times;time [#mu s];hits",  bins, tmin, tmax);
  	// man->CreateH1("hS2Time",  "S2 photon times;time [#mu s];hits",  bins, tmin, tmax);

	// man->OpenFile("output"+strRunID.str()+".root");
	
	man->CreateNtuple("Events", "Golden paramter");
    man->CreateNtupleDColumn("logS2");
    man->CreateNtupleDColumn("S1");
    man->CreateNtupleIColumn("recoilType"); // 0=ER, 1=NR
    man->FinishNtuple(4);
}

void MyRunAction::EndOfRunAction(const G4Run*)
{
	DriftTrackIDs.clear();


	// G4cout << "Number of S1 events: " << nS1Events << G4endl;
	// G4cout << "Number of S2 events: " << nS2Events << G4endl;

    G4cout << "Total S1 Photons: " << totalS1Photons << G4endl;
	G4cout << "Total S2 Photons: " << totalS2Photons << G4endl;

	// G4cout << "Yield Photons: " << yieldPhotons << G4endl;
	// G4cout << "Yield Electrons: " << yieldElectrons << G4endl;

	nS1Events = 0;
    nS2Events = 0;
    totalS1Photons = 0;
    totalS2Photons = 0;
	yieldPhotons = 0;
	yieldElectrons = 0;
	
	G4AnalysisManager *man = G4AnalysisManager::Instance();
	
	man->Write();
	man->CloseFile();
}
