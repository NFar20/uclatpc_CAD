#include "stepping.hh"
#include <cstdlib>

G4ThreadLocal std::unique_ptr<MyDetector> gMyDet;
G4ThreadLocal std::unique_ptr<NEST::NESTcalc> gNest;

MySteppingAction::MySteppingAction() //MyEventAction *eventAction)
{
	if (!gMyDet)  gMyDet  = std::make_unique<MyDetector>();
  if (!gNest)   gNest   = std::make_unique<NEST::NESTcalc>(gMyDet.get());
	//fEventAction = eventAction;
}

MySteppingAction::~MySteppingAction()
{}

int createdElectrons = 0;
int nS1Events = 0;
int nS2Events = 0;
int totalS1Photons = 0;
int totalS2Photons = 0;
int yieldPhotons = 0;
int yieldElectrons = 0;
double gainAreaTop = 32.; //in mm
double gainAreaBottom = 50.2; //in mm
G4double lastStep = 10000.*mm;
int photPerE = 30; //need to find what the actual value should be; 30 is just an approximation
G4ThreadLocal std::unordered_set<G4int> DriftTrackIDs;

void MySteppingAction::ClearStagnationData(G4int trackID)
{
	previousEnergy.erase(trackID);
	stagnationCounter.erase(trackID);
}

int safeRound(double val) {
	if (val > 2000000000.) {
		return 2000000000;
	}
	else {
		return std::round(val);
	}
}

void MySteppingAction::UserSteppingAction(const G4Step *step)
{		
	// Prevent runaway memory usage
	const size_t maxMapSize = 10000;
	if (previousEnergy.size() > maxMapSize) 
	{
		G4cout << "Clearing previousEnergy map!" << G4endl;
		previousEnergy.clear();
		stagnationCounter.clear();
	}

	G4LogicalVolume *volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
	G4Track *track = step->GetTrack();

    // Get position of interaction
    G4ThreeVector pos = step->GetPreStepPoint()->GetPosition();
	G4ThreeVector dir = step->GetPreStepPoint()->GetMomentumDirection();
	double r = std::sqrt(pos.x()/mm * pos.x()/mm + pos.y()/mm * pos.y()/mm);
	const MyDetectorConstruction *detectorConstruction = static_cast<const MyDetectorConstruction*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
	
	G4LogicalVolume *fScoringVolume = detectorConstruction->GetScoringVolume();

	nestPart *nestDetector = new nestPart();
	VDetector *MyDet = new MyDetector();

	G4int id = track->GetTrackID();
	G4double energy = track->GetKineticEnergy();
	const G4ParticleDefinition* pd = track->GetParticleDefinition();
	auto incidentParticle = pd->GetParticleName();
	G4double edep = step->GetTotalEnergyDeposit() / keV;
	static std::map<G4int, G4double> previousEnergy;
	static std::map<G4int, int> stagnationCounter;
	const int maxSteps = 200;
	const double tol = 0.1 * eV;
	bool S2Event = false;
	bool driftElectron = false;
	auto info = dynamic_cast<DriftElectronInfo*>(track->GetUserInformation());
	G4double stepLength = step->GetStepLength();
	G4double nPhotons = 0;
	G4double nElectrons = 0;
	double density = 3.0558; //3.0558*g/cm3
	double temp = 162; //162 Kelvin
	double pressure = 1; //1 Bar (atmospheric pressure)
	static nestPart detector;
	NEST::NESTcalc* nestCalc = new NEST::NESTcalc(MyDet);
	double Efield = nestDetector->get_ElectricField(pos.x() / cm, pos.y() / cm, pos.z() / cm);
	G4int parentID = track->GetParentID();
    G4int trackID = track->GetTrackID();
	NEST::INTERACTION_TYPE recoilType = NEST::NoneType;

	const G4ParticleDefinition* particleDefinition = track->GetParticleDefinition();

	//get rid of particles with extremely low energy
	if(energy < 0.001*eV) {
		track->SetTrackStatus(fStopAndKill);
    	return;
	}

	// If parent is in the drift list, this is a descendant
    if (DriftTrackIDs.count(parentID) && parentID > 0)
    {
        // add this track to the list for future checks
        DriftTrackIDs.insert(trackID);

        // kill it
        track->SetTrackStatus(fStopAndKill);
        G4cout << "Killed track " << trackID << " from drift parent " << parentID << G4endl;
        return;
    }

    if (info && info->IsDrift()) {
        driftElectron = true;
		//track->SetKineticEnergy(energy / 1.25);
		track->SetKineticEnergy(0.1*eV);
		if (volume && (volume->GetName() == "logicWorld" || volume->GetName() == "logicPMT" || volume->GetName() == "logicBottomPMT" || volume->GetName() == "logicTopCap" || volume->GetName() == "logicBottomPMTCap"))
		{
			track->SetTrackStatus(fStopAndKill);
		}
		// return;
    }

	if (pd == G4Electron::ElectronDefinition())
	{
    // Estimate probability of ionization 
		double P_ionize = 0;
		if (pos.z()*mm <= 49.2 && pos.z()*mm > 30)
		{
			P_ionize = 1.;
		}

    	if (G4UniformRand() < P_ionize)
    	{
			edep = energy / keV;

        	track->SetTrackStatus(fStopAndKill);
        	track->SetKineticEnergy(0.0);

        	//G4cout << "Thermal e- ionized by MC model! Deposited " << edep *1000 << " eV" << G4endl;
			ClearStagnationData(id);
    	}
		else
		{
    		if(previousEnergy.count(id))
    		{
				//G4cout << "Starting process..." << G4endl;
    			G4double prevE = previousEnergy[id];        
        		if(std::abs(energy - prevE) < tol || energy < 0.01*eV)
        		{
					//G4cout << "energy loss < total" << G4endl;
            		stagnationCounter[id]++;
        		}
        		else
        		{
					//G4cout << "energy loss > total" << G4endl;
            		stagnationCounter[id] = 0;
        		}
        
        		if(stagnationCounter[id] >= maxSteps)
        		{
            		track->SetTrackStatus(fStopAndKill);
					//G4cout << "killing: " << id << G4endl;
					ClearStagnationData(id);	
        			return;
       			}
    		}
    		else
    		{
				//G4cout << "Did not start process" << G4endl;
        		stagnationCounter[id] = 0;
    		}
			previousEnergy[id] = energy;

		}

	}
	else
	{
    	if(previousEnergy.count(id))
    	{
			//G4cout << "Starting process... (2.0)" << G4endl;
    		G4double prevE = previousEnergy[id];        
        	if(std::abs(energy - prevE) < tol || energy < 0.005*eV)
        	{
				//G4cout << "energy loss < total (2.0)" << G4endl;
           		stagnationCounter[id]++;
        	}
        	else
        	{
				//G4cout << "energy loss > total (2.0)" << G4endl;
           		stagnationCounter[id] = 0;
        	}
        
        	if(stagnationCounter[id] >= maxSteps)
        	{
            	track->SetTrackStatus(fStopAndKill);
				//G4cout << "killing: (2.0) " << pd << G4endl;
				ClearStagnationData(id);	
        		return;
       		}
    	}
    	else
    	{
			//G4cout << "Did not start process (2.0)" << G4endl;
       		stagnationCounter[id] = 0;
    	}
		previousEnergy[id] = energy;		
	}
	
	if(track->GetCurrentStepNumber() >= 20000 || energy < 0.005*eV) 
	{
		track->SetTrackStatus(fStopAndKill);
		ClearStagnationData(id);
	}

	if (pd == G4Neutron::NeutronDefinition() || pd == G4Proton::ProtonDefinition() || pd->GetParticleType() == "nucleus") 
	{
 		recoilType = NEST::INTERACTION_TYPE::NR; // Nuclear Recoil
		incidentParticle = "neutron";
	}
	else if (pd == G4Electron::ElectronDefinition() || pd->GetParticleName() == "ion")
	{
		recoilType = NEST::INTERACTION_TYPE::ion;
		incidentParticle = "electron";
	}
	else if (pd == G4Electron::ElectronDefinition() || pd == G4Positron::PositronDefinition() || pd->GetParticleType() == "muon" || pd->GetParticleType() == "tau")
	{
		recoilType = NEST::INTERACTION_TYPE::beta;
	}
	else if(pd == G4Gamma::GammaDefinition())
	{
		recoilType = NEST::INTERACTION_TYPE::gammaRay;
	}
	else if (pd == G4OpticalPhoton::OpticalPhotonDefinition())
	{
		recoilType = NEST::INTERACTION_TYPE::gammaRay;
		incidentParticle =  "optical photon";
		return;
	}

	if (!std::isfinite(Efield) || Efield < 0.0) return;

	//Nest Part

  	if (volume->GetName() == "logicLXe" || volume->GetName() == "logicGXe")
	{

		if (particleDefinition == G4Electron::ElectronDefinition() || particleDefinition->GetParticleName() == "e-")
		{
			if (volume->GetName() == "logicGXe" || (pos.z()*mm <= 50.2 && pos.z()*mm >= 32))
			{
				check++;
				nPhotons = photPerE;
				nElectrons = 0;
				S2Event = true;
				nS2Events++;
				G4cout << "--------------initializing S2 event--------------" << G4endl;
			}
		}

		// if (edep <= 14.*pow(10,-3) && !driftElectron) //14*10e-3.  14 eV needed for scintillation in liquid xenon   been using 12.*10e-2
		// {
		// 	return;

			// if (edep > 0)
			// {
				
			// 	// track->SetTrackStatus(fStopAndKill);
			// 	// ClearStagnationData(id);
			// 	// G4cout << "Killing because of weird edep code????" << G4endl;

			// 	const G4VProcess* proc = step->GetPostStepPoint()->GetProcessDefinedStep();
    		// 	if (proc) 
			// 	{
        	// 		G4String procName = proc->GetProcessName();
        	// 		//G4cout << "Previous process: " << procName << G4endl;
    		// 	} 
			// 	else 
			// 	{
			// 		//G4cout << "No process defined this step" << G4endl;
			// 	}
			// }


			// return;
		// }


		if(!S2Event)
		{
			// Create NEST object
			G4cout << "~~~~~~~~~~~~~~~initializing S1 event~~~~~~~~~~~~~~~" << G4endl;

			NEST::YieldResult yields = nestCalc->GetYields(recoilType, edep, density, Efield,          // drift field
			131,                     // A
			54                           // Z
			);
			nPhotons = safeRound(yields.PhotonYield);
			nElectrons = safeRound(yields.ElectronYield);
			G4cout << "Number of yield electrons: " << nElectrons << G4endl;
			G4cout << "Number of yield photons: " << nPhotons << G4endl;
			// G4cout << yields.PhotonYield << G4endl;
			// G4cout << yields.ElectronYield << G4endl;
			yieldElectrons = nElectrons;
			yieldPhotons = nPhotons;

			G4cout << "position: " << pos << G4endl;
			G4cout << "Efield: " << Efield << G4endl;
			G4cout << "edep (keV): " << edep << G4endl;
			G4cout << "recoil type: " << recoilType << G4endl;
			G4cout << "Photon Yield: " << nPhotons << G4endl;
			G4cout << "Electron Yield: " << nElectrons << G4endl;
			G4cout << "Incident Particle: " << incidentParticle << G4endl;
			G4cout << "Particle Energy: " << energy/keV << " keV" << G4endl;
			G4cout << "Track ID: " << trackID << G4endl;
			G4cout << "   " << G4endl;
		}

		G4ParticleDefinition* photonDef = G4OpticalPhoton::OpticalPhotonDefinition();
    	G4ParticleDefinition* eDef = G4Electron::ElectronDefinition(); 
			
		G4TrackVector* secondaries = new G4TrackVector();

		//G4cout << "r value: " << r << " Is drift electron? " << driftElectron << G4endl;

    //Spawn optical photons
    	// for (int i = 0; i < nPhotons; ++i) 
		// {

		// 		G4ThreeVector dir = RandomUnitVector();
		// 		G4double photonEnergy = SampleLXePhotonEnergy_GaussEnergy();  

		// 		G4DynamicParticle* dynPart = new G4DynamicParticle(photonDef, dir, photonEnergy);
		// 		G4ThreeVector perp = dir.orthogonal();
		// 		G4double phi = CLHEP::twopi * G4UniformRand();
		// 		G4ThreeVector pol = perp.rotate(dir, phi).unit();
		// 		dynPart->SetPolarization(pol);

		// 		G4Track* newTrack = new G4Track(dynPart, track->GetGlobalTime(), pos);
		// 		newTrack->SetTouchableHandle(track->GetTouchableHandle());
		// 		newTrack->SetParentID(track->GetTrackID());

		// 		secondaries->push_back(newTrack);

    	// }

    //Spawn electrons
    	for (int i = 0; i < nElectrons; ++i) 
		{

        	G4ThreeVector dir = RandomUnitVector();
        	G4double electronEnergy = 0.1*eV;

        	G4DynamicParticle* dynPart = new G4DynamicParticle(eDef, dir, electronEnergy);
        	G4Track* newTrack = new G4Track(dynPart, track->GetGlobalTime(), pos);
			newTrack->SetUserInformation(new DriftElectronInfo(true));
        	newTrack->SetTouchableHandle(track->GetTouchableHandle());
        	newTrack->SetParentID(track->GetTrackID());
			DriftTrackIDs.insert(newTrack->GetTrackID());

        	secondaries->push_back(newTrack);

			createdElectrons = createdElectrons + 1;
			//cout << "------------------------Number of Created Electrons: " << createdElectrons << G4endl;

    	}

    	if (!secondaries->empty()) 
		{
			G4TrackVector* trackSecondaries = const_cast<G4Step*>(step)->GetfSecondary();
        	trackSecondaries->insert(trackSecondaries->end(), secondaries->begin(), secondaries->end());
			//cout << "done creating secondaries" << G4endl;
    	}

		if(S2Event)
		{
			// nS2Events++;
			totalS2Photons = totalS2Photons + nPhotons;
		}
		else
		{
			nS1Events++;
			totalS1Photons = totalS1Photons + nPhotons;
		}

		if(S2Event)
		{
			track->SetTrackStatus(fStopAndKill);
			ClearStagnationData(id);
		}
    	delete secondaries;

    }


	previousEnergy[id] = energy;

	if(volume->GetName() == "logicPMT" || volume->GetName() == "logicBottomPMT" || volume->GetName() == "logicTopCap" || volume->GetName() == "logicBottomPMTCap") {
		track->SetTrackStatus(fStopAndKill);
	}

	//fEventAction->AddEdep(edep);
}