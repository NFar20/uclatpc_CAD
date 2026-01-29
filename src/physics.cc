#include "physics.hh"

MyPhysicsList::MyPhysicsList()
{
	//included physics lists
	
	RegisterPhysics (new G4EmStandardPhysics());
	RegisterPhysics (new G4OpticalPhysics());
	RegisterPhysics (new G4DecayPhysics());
	RegisterPhysics (new G4RadioactiveDecayPhysics());
	RegisterPhysics (new G4HadronElasticPhysicsHP());
	RegisterPhysics (new G4HadronPhysicsQGSP_BIC_HP());
	RegisterPhysics (new G4IonElasticPhysics());
	RegisterPhysics (new G4IonPhysics());
	RegisterPhysics (new G4StoppingPhysics());
	RegisterPhysics (new G4EmLowEPPhysics());
	RegisterPhysics (new G4StepLimiterPhysics());
	// RegisterPhysics (new G4EmExtraPhysics());
	//RegisterPhysics (new G4HadronPhysicsQGSP_BERT());
	//RegisterPhysics (new DarkMatterPhysics());

  	// Turn OFF G4 scintillation so NEST (or custom process) is the sole photon source
  	auto* opticalParams = G4OpticalParameters::Instance();
  	//opticalParams->SetProcessActivation("Scintillation", false);
	opticalParams->SetProcessActivation("Cerenkov", false); //turn off Cerenkov because generates too many optical photons
  	//opticalParams->SetScintStackPhotons(false);

	G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(.1*eV, 1.*GeV);
}

MyPhysicsList::~MyPhysicsList()
{}

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

	AddTransportation();

	if (!det)  det  = new MyDetector();
  	if (!calc) calc = new NEST::NESTcalc(det);

	G4ParticleDefinition *particle;
	
    G4ProcessManager* pm = nullptr;

	auto it = GetParticleIterator();
	it->reset();

	while((*it)()){
    	particle = it->value();
    	pm = particle->GetProcessManager();

		if (particle->GetParticleName() == "e-") 
        {
            G4ProcessManager* pManager = particle->GetProcessManager();
            G4ProcessVector* processVector = pManager->GetProcessList();
            for (int i = 0; i < processVector->size(); i++) 
            {
                G4VProcess* proc = (*processVector)[i];
                if (proc->GetProcessName() == "eIoni") 
                {
                    pManager->RemoveProcess(proc);
                    G4cout << "Removed ionization!!!!!!" << G4endl;
                    break;
                }
                // if (proc->GetProcessName() == "msc")
                // {
                //     //pManager->RemoveProcess(proc);
                //     //G4cout << "Removed msc!!!!!!" << G4endl;
                // }
            }
			auto* fastSimProc = new G4FastSimulationManagerProcess("fastSimProc");
            pManager->AddProcess(fastSimProc, -1, -1, 1);
            G4cout << "Added FastSim process to electron" << G4endl;

      		//limit maximum step size for electrons to 10 mm
			if (pm) pm->AddDiscreteProcess(new G4StepLimiter());
        }
	
		NEST::NESTProc* theNEST2ScintillationProcess = new NEST::NESTProc("S1", fElectromagnetic, det);
		if (theNEST2ScintillationProcess->IsApplicable(*particle)) {
   			pm->AddProcess(theNEST2ScintillationProcess, ordDefault + 1, ordInActive, ordDefault + 1);
		}

	
	}


}

//void MyPhysicsList::SetCuts() { SetCutsWithDefault(); }
