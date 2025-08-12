#include "stepping.hh"

MySteppingAction::MySteppingAction(MyEventAction *eventAction)
{
	fEventAction = eventAction;
	det = new VDetector();
	calc = new NEST::NESTcalc(det);
}

MySteppingAction::~MySteppingAction()
{}

void MySteppingAction::UserSteppingAction(const G4Step *step)
{		
	G4LogicalVolume *volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
	
	const MyDetectorConstruction *detectorConstruction = static_cast<const MyDetectorConstruction*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
	
	G4LogicalVolume *fScoringVolume = detectorConstruction->GetScoringVolume();
	
	G4Track *track = step->GetTrack();
	G4int id = track->GetTrackID();
	G4double energy = track->GetKineticEnergy();
	
	const G4double tol = 1e-6 * MeV;
	const int maxSteps = 100;

	//det->efield = 100.;
	//det->detectorType = 1;
	det->set_E_gas(10000.);
	//det->useS2 = true;

	const G4ParticleDefinition *pd = track->GetParticleDefinition();
	
	if(previousEnergy.count(id))
	{
		G4double prevE = previousEnergy[id];
	
		if(std::abs(energy - prevE) < tol)
		{
			stagnationCounter[id]++;
		}
		else
		{
			stagnationCounter[id] = 0;
		}
		
		if(stagnationCounter[id] >= maxSteps)
		{
			G4cout << "Manually killing stagnant track " << id << " with energy " << energy*eV << " eV."<< G4endl;
			track->SetTrackStatus(fStopAndKill);
		}
	}
	else
	{
		stagnationCounter[id] = 0;
	}
	
	previousEnergy[id] = energy;
	
	if(volume != fScoringVolume) {
		//G4cout << volume->GetName() << G4endl;
		//G4cout << fScoringVolume->GetName() << G4endl;
		return;
	}
	
	G4double edep = step->GetTotalEnergyDeposit();
	fEventAction->AddEdep(edep);

	bool isNR = pd == G4Neutron::Definition();
	bool isER = pd == G4Electron::Definition();

	double edep1 = static_cast<double>(edep / keV);

	if(isER)
	{
		auto yields = calc->GetYields(NEST::ion, edep1, 100., 3., 132., 54.);
    	nPhotons = G4Poisson(floor(yields.PhotonYield / 1000));
    	nElectrons = G4Poisson(floor(yields.ElectronYield / 1000));
		G4cout << "ER, number of photons: " << nPhotons << G4endl;
	}
	if(isNR)
	{
		auto yields = calc->GetYields(NEST::NR, edep1, 100. /* field in V/cm */, 3., 132., 54.);
        nPhotons = G4Poisson(floor(yields.PhotonYield / 1000));
        nElectrons = G4Poisson(floor(yields.ElectronYield / 1000));
		G4cout << "NR, number of photons: " << nPhotons << G4endl;
	}

	//spawn photons
	for (int i = 0; i < nPhotons; i++) {
    	G4ThreeVector pos = step->GetPostStepPoint()->GetPosition();  // spawn at interaction site

    	// Random direction (isotropic emission)
    	G4double theta = acos(1 - 2*G4UniformRand());
    	G4double phi = 2 * CLHEP::pi * G4UniformRand();
    	G4ThreeVector dir(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));

    	// Create optical photon
    	G4ParticleDefinition* opticalPhoton = G4OpticalPhoton::Definition();
    	G4DynamicParticle* dynParticle = new G4DynamicParticle(opticalPhoton, dir, 1.0 * eV);  // example 1 eV

    	// Create primary vertex
    	G4PrimaryParticle* primaryPhoton = new G4PrimaryParticle(dynParticle->GetDefinition());
    	G4PrimaryVertex* vertex = new G4PrimaryVertex(pos, 0.0);
    	vertex->SetPrimary(primaryPhoton);

    	G4EventManager::GetEventManager()->GetNonconstCurrentEvent()->AddPrimaryVertex(vertex);
	}
		//spawn electrons
	for (int i = 0; i < nElectrons; ++i) {
		G4ThreeVector pos = step->GetPostStepPoint()->GetPosition();
		G4ThreeVector dir(0, 0, 1);  // assume drift upward in +z

    	G4ParticleDefinition* e_def = G4Electron::Definition();
    	G4DynamicParticle* dynElectron = new G4DynamicParticle(e_def, dir, 1*eV);

   		G4PrimaryParticle* primaryElectron = new G4PrimaryParticle(dynElectron->GetDefinition());
   		G4PrimaryVertex* vertex = new G4PrimaryVertex(pos, 0.0);
		vertex->SetPrimary(primaryElectron);

		G4EventManager::GetEventManager()->GetNonconstCurrentEvent()->AddPrimaryVertex(vertex);
	}

	//S2 photons
	/*NEST::YieldResult s2yields = calc->GetS2Yield(yields.ElectronYield, *det);
	int nS2Photons = G4Poisson(s2yields.PhotonYield);

	for (int i = 0; i < nS2Photons; ++i) {
    	G4ThreeVector s2pos = interactionPos + G4ThreeVector(0, 0, driftDistance);
    	G4double theta = acos(1 - 2 * G4UniformRand());
    	G4double phi = 2 * pi * G4UniformRand();
    	G4ThreeVector dir(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));

    	G4DynamicParticle* dynPhoton = new G4DynamicParticle(G4OpticalPhoton::Definition(), dir, 7.0 * eV);
    	G4PrimaryParticle* primary = new G4PrimaryParticle(dynPhoton);
    	G4PrimaryVertex* vertex = new G4PrimaryVertex(s2pos, driftTime);  // you can compute driftTime

    	vertex->SetPrimary(primary);
    	G4EventManager::GetEventManager()->GetNonconstCurrentEvent()->AddPrimaryVertex(vertex);
}*/

}


void MySteppingAction::ClearStagnationData(G4int trackID)
{
	previousEnergy.erase(trackID);
	stagnationCounter.erase(trackID);
}
