/*#include "G4HadronElasticProcess.hh"
#include "G4VDiscreteProcess.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ParticleChange.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

class WIMPElasticProcess : public G4VDiscreteProcess
{
public:
	WimpElasticProcess() : G4VDiscreteProcess("WIMPElastic")
	{}
	
	G4bool IsApplicable(const G4ParticleDefinition* particle) override
	{
		return particle->GetParticleName() == "WIMP";
	}
	
	G4double GetMeanFreePath(const G4Track&, G4double, G4ForceCondition *condition) override
	{
		*condition = NotForced;
		return 1.*m; //get a real mean free path based on cross section
	}
	
	G4VParticleChange *PostStepDoIt(const G4Track& track, const G4Step& step) override
	{
		fParticleChange.Initialize(track);
		
		G4double recoilE = G4UniformRand() * 50. * keV;
		
		fParticleChange.ProposeTrackStatus(fStopAndKill);
		
		G4DynamicParticle *recoil = new G4DynamicParticle(G4ParticleTable::GetParticleTable()->FindParticle("Xe132"), G4ThreeVector(1, 0, 0) dummy direction, recoilE);
		fParticleChange.ProposeSecondary(recoil);
		
		return &fParticleChange;
		
	}
};*/
