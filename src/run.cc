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

	// man->CreateNtuple("tS2-tS1", "global_electron_drift_time");
	// man->CreateNtupleDColumn("globalTime_us");
	// man->CreateNtupleIColumn("trackID");
	// man->FinishNtuple(2);

	// man->CreateNtuple("drift", "electron_drift_time");
	// man->CreateNtupleDColumn("driftTime_us");
	// // man->CreateNtupleIColumn("distance_mm");
	// man->FinishNtuple(3);

	// man->CreateNtuple("data", "Energy deposition data for nestpy");
	// man->CreateNtupleIColumn("event_id");      // 0
  	// man->CreateNtupleIColumn("step_id");       // 1
  	// man->CreateNtupleDColumn("edep_keV");      // 2
  	// man->CreateNtupleDColumn("x_mm");          // 3
  	// man->CreateNtupleDColumn("y_mm");          // 4
  	// man->CreateNtupleDColumn("z_mm");          // 5
  	// man->CreateNtupleDColumn("t_ns");          // 6
  	// man->CreateNtupleDColumn("Efield_Vcm");    // 7
  	// man->CreateNtupleIColumn("type");          // 8  (0=ER,1=NR)
  	// man->CreateNtupleIColumn("track_id");      // 9
  	// man->CreateNtupleIColumn("parent_id");     // 10
  	// man->CreateNtupleIColumn("pdg");           // 11
  	// man->CreateNtupleSColumn("process");       // 12
  	// man->CreateNtupleSColumn("volume");        // 13
  	// man->CreateNtupleSColumn("material");      // 14
  	// man->FinishNtuple(4);

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

    // DriftTrackIDs.clear();

	nS2Events = 0;
	nS1Events = 0;
	
	G4AnalysisManager *man = G4AnalysisManager::Instance();
	
	man->SetVerboseLevel(1);
	
	G4int runID = run->GetRunID();
	std::stringstream strRunID;
	strRunID << runID;

	// if (!man->IsOpenFile()) {
    // 	man->OpenFile("output.root");
  	// }

	// man->OpenFile("output"+strRunID.str()+".root");
	man->OpenFile("output.root");

	man->CreateNtuple("photonTimes", "Created photon times");
	// man->CreateNtupleIColumn("event");   // eventID
	man->CreateNtupleIColumn("type");    // 1=S1, 2=S2
	man->CreateNtupleDColumn("t_us");    // creation time in microseconds
	man->FinishNtuple(2);

	man->CreateNtuple("eDrift", "Drift electrons");
	// man->CreateNtupleIColumn("event");
	man->CreateNtupleDColumn("tid");
	man->CreateNtupleDColumn("t_create_us");
	man->CreateNtupleDColumn("t_exitLXe_us");
	man->CreateNtupleDColumn("t_drift_us");
	man->FinishNtuple(3);
	
	
	man->CreateNtuple("Events", "Golden paramter");
    man->CreateNtupleDColumn("logS2");
    man->CreateNtupleDColumn("S1");
    man->CreateNtupleIColumn("recoilType"); // 0=ER, 1=NR
    man->FinishNtuple(4);
}

void MyRunAction::EndOfRunAction(const G4Run*)
{
	// DriftTrackIDs.clear();


	// G4cout << "Number of S1 events: " << nS1Events << G4endl;
	// G4cout << "Number of S2 events: " << nS2Events << G4endl;

    G4cout << "Total S1 Photons: " << totalS1Photons << G4endl;
	G4cout << "Total S2 Photons: " << totalS2Photons << G4endl;

	// G4cout << "Yield Photons: " << yieldPhotons << G4endl;
	// G4cout << "Yield Electrons: " << yieldElectrons << G4endl;

	// nS1Events = 0;
    // nS2Events = 0;
    // totalS1Photons = 0;
    // totalS2Photons = 0;
	// yieldPhotons = 0;
	// yieldElectrons = 0;
	
	G4AnalysisManager *man = G4AnalysisManager::Instance();
	
	man->Write();
	man->CloseFile();
}
