#ifndef DETECTOR_HH
#define DETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4ParticleTable.hh"
#include "G4OpticalPhoton.hh"
#include "G4SystemOfUnits.hh"

#include <set>

class MySensitiveDetector : public G4VSensitiveDetector
{
public:
	MySensitiveDetector(G4String);
	~MySensitiveDetector();
	
	G4ThreeVector posDetector;

private:
	virtual G4bool ProcessHits(G4Step *, G4TouchableHistory *);
	G4double zSurface;
};

#endif
