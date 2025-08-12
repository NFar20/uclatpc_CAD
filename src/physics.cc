#include "physics.hh"

MyPhysicsList::MyPhysicsList()
{
	defaultCutValue = 0.001*CLHEP::mm;
	
	//RegisterPhysics (new G4EmStandardPhysics());
	RegisterPhysics (new G4OpticalPhysics());
	RegisterPhysics (new G4DecayPhysics());
	RegisterPhysics (new G4RadioactiveDecayPhysics());
	RegisterPhysics (new G4HadronElasticPhysicsHP());
	RegisterPhysics (new G4HadronPhysicsQGSP_BIC_HP());
	RegisterPhysics (new G4IonElasticPhysics());
	RegisterPhysics (new G4IonPhysics());
	RegisterPhysics (new G4StoppingPhysics());
	RegisterPhysics (new G4EmLowEPPhysics());
	//RegisterPhysics (new G4EmExtraPhysics());
	//RegisterPhysics (new G4HadronPhysicsQGSP_BERT());
	//RegisterPhysics (new DarkMatterPhysics());

	auto* opt = new G4OpticalPhysics();
  	RegisterPhysics(opt);

  	// Turn OFF Geant4 scintillation so NEST (or your custom process) is the sole photon source
  	auto* opticalParams = G4OpticalParameters::Instance();
  	opticalParams->SetProcessActivation("Scintillation", false);
	opticalParams->SetProcessActivation("Cerenkov", false);
  	// Optional: also ensure Geant4 doesn’t try to stack scint photons
  	opticalParams->SetScintStackPhotons(false);
}

MyPhysicsList::~MyPhysicsList()
{
	delete calc;
  	delete det;
}

void MyPhysicsList::ConstructParticle()
{
  	G4VModularPhysicsList::ConstructParticle();

	G4Geantino::GeantinoDefinition();
  	G4Gamma::GammaDefinition();
  	G4Electron::ElectronDefinition();
  	G4Positron::PositronDefinition();
  	G4GenericIon::GenericIonDefinition();
  	G4Neutron::NeutronDefinition();
  	G4Proton::ProtonDefinition();
	NEST::NESTThermalElectron::Definition();
}

void MyPhysicsList::ConstructProcess()
{
	G4VModularPhysicsList::ConstructProcess();
	G4cout << "[DEBUG] ConstructProcess called" << G4endl;

	if (!det)  det  = new MyDetector();
  	if (!calc) calc = new NEST::NESTcalc(det);

	G4ParticleDefinition *p;
	//auto *particle = it->value();
	//G4ProcessManager *pmanager = particle->GetProcessManager();

	
    G4ProcessManager* pm = nullptr;

	auto it = GetParticleIterator();
	it->reset();

	while((*it)()){
		G4cout << "[DEBUG] While loop is running" << G4endl;
    	p = it->value();
    	pm = p->GetProcessManager();

		if(p->GetParticleName() == "e-" || p == G4Electron::ElectronDefinition())
		{
			pm = p->GetProcessManager();
			G4ProcessVector *processVector = pm->GetProcessList();
			for(int i = 0; i < processVector->size(); i++)
			{
				
				G4VProcess* proc = (*processVector)[i];
				if(proc->GetProcessName() == "eIoni")
				{
					pm->RemoveProcess(proc);
					G4cout << "==============================Removed Ionization==============================" << G4endl;
					break;
				}
				if(proc->GetProcessName() == "msc")
				{
					pm->RemoveProcess(proc);
					G4cout << "==============================Removed msc Process==============================" << G4endl;
				}
				
			}

			auto* nestS1 = new NEST::NESTProc("S1", fElectromagnetic, calc, det);
			if (nestS1->IsApplicable(*p)) {
    			// Place after standard EM ionization; typical ordering:
 				pm->AddProcess(nestS1, ordDefault+1, ordInActive, ordDefault+1);
    		} 
			else {
    			delete nestS1; // avoid leaks for particles where it’s not applicable
    		}
		}

		if (p == NEST::NESTThermalElectron::Definition()) {
            auto* nestS2 = new NEST::NESTProc("S2", fElectromagnetic, calc, det);
            if (nestS2->IsApplicable(*p)) {
                // Discrete process that converts drifted electrons to EL (S2) light in gas
                pm->AddProcess(nestS2, ordDefault+1, ordInActive, ordDefault+1);
            } else {
                delete nestS2;
            }
  		}
	
		//NEST::NESTProc* theNEST2ScintillationProcess = new NEST::NESTProc("S1", fElectromagnetic, calc, det);
		//if (theNEST2ScintillationProcess->IsApplicable(*particle)) {
   		//	pmanager->AddProcess(theNEST2ScintillationProcess, ordDefault + 1, ordInActive, ordDefault + 1);
		//}

	
	}
}

void MyPhysicsList::SetCuts() { SetCutsWithDefault(); }
