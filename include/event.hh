#ifndef EVENT_HH
#define EVENT_HH

#include "G4UserEventAction.hh"
#include "G4Event.hh"

#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"

#include <unordered_map>

#include "run.hh"
// #include "stepping.hh"


class MyEventAction : public G4UserEventAction
{
public:
	MyEventAction(MyRunAction*);
	~MyEventAction();
	
	virtual void BeginOfEventAction(const G4Event*);
	virtual void EndOfEventAction(const G4Event*);
	
	void AddEdep(G4double edep);
	void AddData(G4double edep_keV, G4double x_mm, G4double y_mm, G4double z_mm,
		G4double time,
		G4double Efield_Vcm,
		G4int type, G4int trackID, G4int parentID, G4int pdg,
    	const G4String& process,
    	const G4String& volume,
    	const G4String& material);
	
	void AddTimesLXe(G4double t);
	void AddTimesGXe(G4double t);

	void AddS1PhotonTime(G4double t) { s1PhotonTimes.push_back(t); }
  	void AddS2PhotonTime(G4double t) { s2PhotonTimes.push_back(t); }

  	const std::vector<G4double>& GetS1PhotonTimes() const { return s1PhotonTimes; }
  	const std::vector<G4double>& GetS2PhotonTimes() const { return s2PhotonTimes; }

	void RecordElectronCreate(G4int tid, G4double t) { eCreate[tid] = t; }
  	void RecordElectronExitLXe(G4int tid, G4double t) { eExitLXe.emplace(tid, t); }

private:
	G4double fEdep;
	G4int fEventID = -1;
	G4int fStepCounter = 0;
	std::vector<G4double> times_LXe;
    std::vector<G4double> times_GXe;

	std::vector<G4double> s1PhotonTimes;
  	std::vector<G4double> s2PhotonTimes;

	std::unordered_map<G4int, G4double> eCreate;   // tid -> creation time
  	std::unordered_map<G4int, G4double> eExitLXe;  // tid -> first exit time
};

#endif
