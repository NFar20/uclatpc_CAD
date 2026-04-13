#include "event.hh"

extern int totalS1Photons;
extern int totalS2Photons;
extern int nS2events;
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
    totalS1Photons = 0;
    totalS2Photons = 0;
    // nS2Events = 0;
    fEventID = evt->GetEventID();
    fStepCounter = 0;
    s1PhotonTimes.clear();
    s2PhotonTimes.clear();
    eCreate.clear();
    eExitLXe.clear();
    // times_LXe.clear();
    // times_GXe.clear();
}

void MyEventAction::EndOfEventAction(const G4Event*)
{
	G4cout << "Energy deposition: " << fEdep << G4endl;
    
	G4AnalysisManager *man = G4AnalysisManager::Instance();
    G4RunManager* runManager = G4RunManager::GetRunManager();
    int eventID = runManager->GetCurrentEvent()->GetEventID();
	
    // if(nS2Events > 10000) {
    //     nS2Events = 0; // reset S2 event count for this event
    //     G4cout << "[END OF EVENT ACTION] Stopping event to avoid memory overflow." << G4endl;
    //     G4RunManager::GetRunManager()->AbortRun(true); // stop the run after this event to prevent memory overflow
    // }
    int L = times_LXe.size() - 1;
    int G = times_GXe.size() - 1;

    int c = std::min(L, G);

    for(int i = 0; i < c; i++) {
        G4double t_drift_us = times_GXe[i] - times_LXe[i];
        man->FillNtupleDColumn(3, 0, t_drift_us);
    }
	
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

    for (auto t : s1PhotonTimes) {
        man->FillNtupleIColumn(2, 0, 1);      // type = 1 (S1)
        man->FillNtupleDColumn(2, 1, t/10);   // time in us, should be /1000
        man->AddNtupleRow(2);
    }
    for (auto t : s2PhotonTimes) {
        man->FillNtupleIColumn(2, 0, 2);      
        man->FillNtupleDColumn(2, 1, t/10); //should be
        man->AddNtupleRow(2);
    }

    // if(nS2Events > 10000) {
    //     G4cout << "[EVENT ACTION] Too many S2 events, stopping simulation to prevent memory overflow." << G4endl;
    //     G4RunManager::GetRunManager()->AbortRun(true);
    //  }


    for (const auto& kv : eExitLXe) {
        G4int tid = kv.first;
        G4double t_exit = kv.second;

        auto it = eCreate.find(tid);
        if (it == eCreate.end()) continue; // safety

        G4double t_create = it->second;
        G4double t_drift  = t_exit - t_create;

        // man->FillNtupleIColumn(0, 0, eventID);
        man->FillNtupleDColumn(3, 0, tid);
        man->FillNtupleDColumn(3, 1, t_create/us);
        man->FillNtupleDColumn(3, 2, t_exit/us);
        man->FillNtupleDColumn(3, 3, t_drift/us);
        man->AddNtupleRow(3);
    }

}

void MyEventAction::AddTimesLXe(double t) {
    times_LXe.push_back(t);
}

void MyEventAction::AddTimesGXe(double t) {
    times_GXe.push_back(t);
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

    

    // man->FillNtupleIColumn(4, 0, fEventID);
    // man->FillNtupleIColumn(4, 1, stepID);
    // man->FillNtupleDColumn(4, 2, edep_keV);
    // man->FillNtupleDColumn(4, 3, x_mm);
    // man->FillNtupleDColumn(4, 4, y_mm);
    // man->FillNtupleDColumn(4, 5, z_mm);
    // man->FillNtupleDColumn(4, 6, time);
    // man->FillNtupleDColumn(4, 7, Efield_Vcm);
    // man->FillNtupleIColumn(4, 8, type);
    // man->FillNtupleIColumn(4, 9, trackID);
    // man->FillNtupleIColumn(4, 10, parentID);
    // man->FillNtupleIColumn(4, 11, pdg);

    // man->FillNtupleSColumn(4, 12, process);
    // man->FillNtupleSColumn(4, 13, volume);
    // man->FillNtupleSColumn(4, 14, material);

    // man->AddNtupleRow();

}

