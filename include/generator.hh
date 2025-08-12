#ifndef GENERATOR_HH
#define GENERATOR_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include"G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"
#include "G4Geantino.hh"
#include "G4IonTable.hh"
#include "G4GenericMessenger.hh"
#include "G4IsotopeProperty.hh"

#include "physics.hh"

class MyPrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
public:
	MyPrimaryGenerator();
	~MyPrimaryGenerator();
	
	virtual void GeneratePrimaries(G4Event*) override;
	void SetSource(G4String val);
	
private:
	G4ParticleGun *fParticleGun;
	
	G4GenericMessenger *fMessenger;
	
	G4String source, generic; //type of particle fired
	G4double momMagnitude; // magnitude of particle momentum
	G4int n; //number of particles fired from particle gun
	G4int instanceID;
	
	G4int particleIndex;
	
	G4ParticleDefinition *Co60, *Ba133, *Cf252, *Na22, *Co57, *Cs137, *Se72, *Rn220, *WIMP;
};

#endif
