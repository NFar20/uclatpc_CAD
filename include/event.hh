#ifndef EVENT_HH
#define EVENT_HH

#include "G4UserEventAction.hh"
#include "G4Event.hh"

#include "G4AnalysisManager.hh"

#include "run.hh"

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

private:
	G4double fEdep;
	G4int fEventID = -1;
	G4int fStepCounter = 0;
};

#endif
