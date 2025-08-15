#ifndef PHYSICS_HH
#define PHYSICS_HH

#include "G4VModularPhysicsList.hh"
#include "G4EmStandardPhysics.hh"
#include "G4OpticalPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4IonElasticPhysics.hh"
#include "G4IonPhysics.hh"
#include "G4StoppingPhysics.hh"
#include "G4EmLowEPPhysics.hh"
#include "G4EmExtraPhysics.hh"
//#include "G4HadronPhysicsQGSP_BERT.hh"
//#include "DMG4/DarkMatterPhysics.hh"
#include "generator.hh"
//#include"WIMPElasticProcess.hh"
#include "G4StepLimiterPhysics.hh"

#include "NEST.hh"
#include "NESTProc.hh"
#include "VDetector.hh"
#include "MyDetector.hh"

#include "G4ProcessManager.hh"
#include "G4ParticleTable.hh"
#include "G4Transportation.hh"
#include "G4ProcessVector.hh"
#include "G4OpticalParameters.hh"

#include "G4EmParameters.hh"
#include "G4eMultipleScattering.hh"
#include "G4UrbanMscModel.hh"
#include "G4Electron.hh"
#include "G4OpticalParameters.hh"

class VDetector;
namespace NEST { class NESTcalc; class NESTProc; }

class MyPhysicsList : public G4VModularPhysicsList
{
public:
	MyPhysicsList();
	~MyPhysicsList() override;

	//VDetector *LXeDet;
	//NEST::NESTcalc *calc;
	MyDetector*     GetNESTDetector() const { return static_cast<MyDetector*>(det); }
  	NEST::NESTcalc* GetNESTcalc()     const { return calc; }
	
	void ConstructParticle() override;
	void ConstructProcess() override;
	//void SetCuts() override;

private:
	VDetector* det = nullptr;
	NEST::NESTcalc* calc = nullptr;
};

#endif
