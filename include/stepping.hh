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
#include <unordered_set>

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

extern int photPerE;
extern bool S2Event;
extern double gainArea;
extern G4ThreadLocal std::unordered_set<G4int> DriftTrackIDs;

class MySteppingAction : public G4UserSteppingAction
{
public:
	MySteppingAction(); //MyEventAction* eventAction
	~MySteppingAction();
	
	virtual void UserSteppingAction(const G4Step*);
	
	void ClearStagnationData(G4int trackID);
	
private:
	//MyEventAction *fEventAction;
	NEST::NESTcalc* calc;
	VDetector* det;

	int nPhotons, nElectrons, nS2Photons;
	
	std::map<G4int, G4double> previousEnergy;
	std::map<G4int, G4int> stagnationCounter;
};

inline G4ThreeVector RandomUnitVector() 
{
    double costheta = 2.*G4UniformRand() - 1.;
    double sintheta = std::sqrt(1. - costheta*costheta);
    double phi = 2.*CLHEP::pi*G4UniformRand();
    return G4ThreeVector(sintheta*cos(phi), sintheta*sin(phi), costheta);
}

// samples photon energy in eV with Gaussian smearing
inline G4double SampleLXePhotonEnergy_GaussEnergy() 
{
    const double mean_eV  = 1239.8419843320026/178.0; // ~6.966 eV
    const double sigma_eV = 0.20;                     // from conversion above
    double E_eV = CLHEP::RandGauss::shoot(mean_eV, sigma_eV);
    if (E_eV < 6.5) E_eV = 6.5;
    if (E_eV > 8.2) E_eV = 8.2;
    return E_eV*eV;
}

#endif
