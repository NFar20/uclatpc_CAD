#include "generator.hh"
#include <cmath>

MyPrimaryGenerator::MyPrimaryGenerator()
{
	generic = "geantino";
	particleIndex = -1;
	source = "test";
	
	static G4int instanceCount = 0;
	instanceCount++;
	G4cout << "[MyPrimaryGenerator] Instance created, ID = " << instanceCount << G4endl;
	this->instanceID = instanceCount;
	
	fMessenger = new G4GenericMessenger(this, "/myGen/", "Particle Selection");
	fMessenger->DeclareMethod("setParticle", &MyPrimaryGenerator::SetSource, "Choose particle source");
	fMessenger->DeclareProperty("setGeneric", generic, "Choose generic particle");
	fMessenger->DeclarePropertyWithUnit("setMom", "MeV", momMagnitude, "Set momentum magnitude");
	fMessenger->DeclareProperty("setNum", n, "Set number of particles fired");
	
	G4cout << "[Messenger] Bound to instance ID = " << instanceID << G4endl;
	
	momMagnitude = 1.5*MeV;
	n = 1;
	
	fParticleGun = new G4ParticleGun(n);
	
	G4ParticleDefinition *particle = G4ParticleTable::GetParticleTable()->FindParticle(generic);
	
	G4ThreeVector pos(0., 0.*mm, 77.2*mm);
	G4ThreeVector mom(0., -1., 0.);
	fParticleGun->SetParticlePosition(pos);
	fParticleGun->SetParticleMomentumDirection(mom);
	fParticleGun->SetParticleMomentum(momMagnitude);
	fParticleGun->SetParticleDefinition(particle);
	
	WIMP = new G4ParticleDefinition("WIMP", 50*GeV, 0., 0., 0, 0, 0, 0, 0, 0, "darkmatter", 0, 0, 0, true, 0., nullptr, false, "", 0, 0.);
	
	G4cout << "Constructor source value: " << source << G4endl;
	
	//fParticleGun->SetParticlePolarization(G4ThreeVector(1., 0., 0.));
}

MyPrimaryGenerator::~MyPrimaryGenerator()
{
	delete fParticleGun;
}

void MyPrimaryGenerator::SetSource(G4String val)
{
	G4cout << "[SetSource] Called with value: " << val << G4endl;
	source = val;
	return;
}

void MyPrimaryGenerator::GeneratePrimaries(G4Event *anEvent)
{
	
	if(!Co60)
	{
		Co60 = G4IonTable::GetIonTable()->GetIon(27, 60, 0);
		const_cast<G4ParticleDefinition*>(Co60)->SetPDGStable(false);
		Co60->SetPDGLifeTime(1.66e+08); //short lifetime to test
	}
	
	else if(!Ba133)
	{
		Ba133 = G4IonTable::GetIonTable()->GetIon(56, 133, 0);
		const_cast<G4ParticleDefinition*>(Ba133)->SetPDGStable(false);
	}
	
	else if(!Na22)
	{
		Na22 = G4IonTable::GetIonTable()->GetIon(11, 22, 0);
		const_cast<G4ParticleDefinition*>(Na22)->SetPDGStable(false);
	}
	
	else if(!Cf252)
	{
		Cf252 = G4IonTable::GetIonTable()->GetIon(98, 252, 0);
		const_cast<G4ParticleDefinition*>(Cf252)->SetPDGStable(false);
	}
	
	//AmBe neutron spectrum
	/*G4double E;
	G4double[110] function;
	if(AmBe)
	{
		while(true)
		{
			double E = G4UniformRand() * 11.0;
			
			if(G4UniformRand() < function)
			{
				break;
			}
		}
			fParticleGun->SetParticleMomentum(E*MeV);
	}*/
	
	
	else if(!Co57)
	{
		Co57 = G4IonTable::GetIonTable()->GetIon(27, 57, 0);
		const_cast<G4ParticleDefinition*>(Co57)->SetPDGStable(false);
	}
	
	else if(!Cs137)
	{
		Cs137 = G4IonTable::GetIonTable()->GetIon(55, 137, 0);
		const_cast<G4ParticleDefinition*>(Cs137)->SetPDGStable(false);
	}
	
	//gaseous sources below
	else if(!Se72)
	{
		Se72 = G4IonTable::GetIonTable()->GetIon(34, 72, 0);
		const_cast<G4ParticleDefinition*>(Se72)->SetPDGStable(false);
	}
	
	//unsure how to implement tritiated methane
	
	else if(!Rn220)
	{
		Rn220 = G4IonTable::GetIonTable()->GetIon(86, 220, 0);
		const_cast<G4ParticleDefinition*>(Rn220)->SetPDGStable(false);
	}
	
	G4bool AmBe = false;

	G4ParticleDefinition *particle = fParticleGun->GetParticleDefinition();
	
	G4ParticleDefinition* particleArray[9];
	particleArray[0] = Ba133;
	particleArray[1] = Na22;
	particleArray[2] = Cf252;
	particleArray[3] = Co57;
	particleArray[4] = Co60;
	particleArray[5] = Cs137;
	particleArray[6] = Se72;
	particleArray[7] = Rn220;
	//particleArray[8] = G4ParticleTable::GetParticleTable()->FindParticle("geantino");
	particleArray[8] = G4ParticleTable::GetParticleTable()->FindParticle("e-");
	
	if(source == "Ba133")
		particleIndex = 0;
	else if(source == "Na22")
		particleIndex = 1;
	else if(source == "Cf252")
		particleIndex = 2;
	else if(source == "Co57")
		particleIndex = 3;
	else if(source == "Co60")
		particleIndex = 4;
	else if(source == "Cs137")
		particleIndex = 5;
	else if(source == "Se72")
		particleIndex = 6;
	else if(source == "Rn220")
		particleIndex = 7;
	else
	{
		particleIndex = 8;
		//G4cout << source << G4endl;	
	}
	G4cout << source << G4endl;
	//G4cout << particleIndex << G4endl;
	G4cout << "[GeneratePrimaries] Called on instance ID = " << instanceID << ", source = " << source << G4endl;
	
	if(particleIndex >= 0 && particleIndex <=8 && particleArray[particleIndex] != nullptr)
	{
		fParticleGun->SetParticleDefinition(particleArray[particleIndex]);
		fParticleGun->SetParticleCharge(0.*eplus);
		G4cout << source << G4endl;
	}
	else
	{
		fParticleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle("geantino"));
		G4cout << "Invalid choice of particle" << G4endl;
	}
	
	fParticleGun->GeneratePrimaryVertex(anEvent);
}
