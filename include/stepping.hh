#ifndef STEPPING_HH
#define STEPPING_HH

#include "G4UserSteppingAction.hh"
#include"G4Step.hh"
#include "G4Track.hh"
#include "globals.hh"
#include "G4Neutron.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4OpticalPhoton.hh"
#include "G4DynamicParticle.hh"
#include "G4Poisson.hh"
#include <map>
#include <cmath>

#include "G4StackManager.hh"

#include "NEST.hh"
#include "nestPart.hh"
#include "NESTProc.hh"
#include "Detectors/VDetector.hh"
#include "MyDetector.hh"
#include "DriftElectronInfo.hh"
#include "DynamicUserLimits.hh"
#include "G4VProcess.hh"
#include "CLHEP/Random/RandGaussZiggurat.h"

#include "construction.hh"
#include "event.hh"
#include "tracking.hh"

class MySteppingAction : public G4UserSteppingAction
{
public:
	MySteppingAction(MyEventAction* eventAction);
	~MySteppingAction();
	
	virtual void UserSteppingAction(const G4Step*);
	
	void ClearStagnationData(G4int trackID);
	
private:
	MyEventAction *fEventAction;
	NEST::NESTcalc* calc;
	VDetector* det;

	int nPhotons, nElectrons, nS2Photons;
	
	std::map<G4int, G4double> previousEnergy ;
	std::map<G4int, G4int> stagnationCounter;
};

#endif
