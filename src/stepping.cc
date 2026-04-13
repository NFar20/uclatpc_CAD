#include "stepping.hh"
#include <cstdlib>

G4ThreadLocal std::unique_ptr<MyDetector> gMyDet;
G4ThreadLocal std::unique_ptr<NEST::NESTcalc> gNest;

MySteppingAction::MySteppingAction(MyEventAction *eventAction) //
{
	if (!gMyDet)  gMyDet  = std::make_unique<MyDetector>();
	if (!gNest)   gNest   = std::make_unique<NEST::NESTcalc>(gMyDet.get());
	fEventAction = eventAction;
}

MySteppingAction::~MySteppingAction()
{}

int createdElectrons = 0;
int nS1Events = 0;
int nS2Events = 0;
int totalS1Photons;
int totalS2Photons;
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
	if (val > 10000.) {
		return 10000;
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

	G4VPhysicalVolume* pVol = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
	G4VPhysicalVolume* testVol = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
	if(!pVol || !testVol || (testVol == nullptr) || (pVol == nullptr)) {
		G4cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~No volume found!~~~~~~~~~~~~~~~~~~~~~~~~~~~" << G4endl;
		return;
	}

	if(nS2Events > 10000) {
		G4cout << "Too many S2 events, stopping simulation to prevent memory overflow." << G4endl;
		// G4RunManager::GetRunManager()->AbortEvent();
		G4RunManager::GetRunManager()->AbortRun();
		nS2Events = 0;
		return;
	}
	G4LogicalVolume *volume = pVol->GetLogicalVolume();
	G4Track *track = step->GetTrack();

    // Get position of interaction
    G4ThreeVector pos = step->GetPreStepPoint()->GetPosition();
	G4ThreeVector dir = step->GetPreStepPoint()->GetMomentumDirection();
	double r = std::sqrt(pos.x()/mm * pos.x()/mm + pos.z()/mm * pos.z()/mm);
	const MyDetectorConstruction *detectorConstruction = static_cast<const MyDetectorConstruction*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
	
	G4LogicalVolume *fScoringVolume = detectorConstruction->GetScoringVolume();

	nestPart *nestDetector = new nestPart();
	VDetector *MyDet = new MyDetector();

	G4int id = track->GetTrackID();
	G4double energy = track->GetKineticEnergy();
	const G4ParticleDefinition* pd = track->GetParticleDefinition();
	auto incidentParticle = pd->GetParticleName();
	G4double energyDeposit = step->GetTotalEnergyDeposit();
	G4double edep = energyDeposit * 1000; //* 1000
	static std::map<G4int, G4double> previousEnergy;
	static std::map<G4int, int> stagnationCounter;
	const int maxSteps = 200;
	const double tol = 0.1 * eV;
	bool S2Event = false;
	bool S1Event = false;
	bool driftElectron = false;
	auto info = dynamic_cast<DriftElectronInfo*>(track->GetUserInformation());
	G4double stepLength = step->GetStepLength();
	G4double nPhotons = 0;
	G4double nElectrons = 0;
	double density = 3.0558; //3.0558*g/cm3
	double temp = 162; //162 Kelvin
	double pressure = 1.013; //1 Bar (atmospheric pressure)
	static nestPart detector;
	NEST::NESTcalc *nestCalc = new NEST::NESTcalc(MyDet);
	static NEST::NESTcalc nCalc(&detector);
	double Efield = nestDetector->get_ElectricField(pos.x(), pos.y(), pos.z());
	G4int parentID = track->GetParentID();
    G4int trackID = track->GetTrackID();
	G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
	G4int stepNo = track->GetCurrentStepNumber();
	NEST::INTERACTION_TYPE recoilType = NEST::NoneType;
	G4int S1phot = 0;
	const G4double t0 = step->GetPreStepPoint()->GetGlobalTime();

	G4LogicalVolume *postVol = nullptr;
	auto man = G4AnalysisManager::Instance();
	G4bool isPhot = (pd == G4OpticalPhoton::OpticalPhotonDefinition());
	G4double t_S1 = 0.;
	G4double y_S1 = 0.;

	const G4ParticleDefinition* particleDefinition = track->GetParticleDefinition();

	if(pd==G4OpticalPhoton::OpticalPhotonDefinition()) {
		// G4cout << "Optical Photon detected in " << volume->GetName() << G4endl;
	}

	//get rid of particles with extremely low energy
	// if(energy < 0.001*eV && !isPhot) {
	// 	track->SetTrackStatus(fStopAndKill);
    // 	return;
	// }

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
		if (volume && (volume->GetName() == "logicTopCap" || volume->GetName() == "logicBottomPMTCap" || volume->GetName() == "logicBottomPMT"))
		{
			// volume->GetName() == "logicWorld"
			track->SetTrackStatus(fStopAndKill);
			G4cout << "killing drift e- that escaped TPC" << G4endl;
		}
		// if(volume->GetName() == "logicPMT0" || volume->GetName() == "logicPMT1" || volume->GetName() == "logicPMT2" || volume->GetName() == "logicPMT3" || volume->GetName() == "logicPMT4" || volume->GetName() == "logicPMT5" || volume->GetName() == "logicPMT6")
		// {
		// 	track->SetTrackStatus(fStopAndKill);
		// 	G4cout << "killing drift e- that entered PMT" << G4endl;
		// }
		// else if(volume->GetName() == "logicTopCap" || volume->GetName() == "logicBottomPMTCap" || volume->GetName() == "logicBottomPMT")
		// {
		// 	track->SetTrackStatus(fStopAndKill);
		// 	G4cout << "killing drift e- that entered PMT Cap" << G4endl;
		// }

		// return;
    }

	auto post = step->GetPostStepPoint();
	if(post && driftElectron) {
		if(post->GetTouchableHandle()->GetVolume()->GetLogicalVolume() != nullptr && volume->GetName() != "logicWorld") {
			postVol = post->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
		}
	}

	if (pd == G4Electron::ElectronDefinition() && !driftElectron)
	{
    // Estimate probability of ionization 
		double P_ionize = 0;
		if (pos.y() <= -26.*mm && pos.y() >= -44.*mm)
		{
			P_ionize = 0; //0.1
		}

    	if (G4UniformRand() < P_ionize)
    	{
			edep = energy; //* 1000

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
		isPhot = true;
		recoilType = NEST::INTERACTION_TYPE::gammaRay;
		incidentParticle =  "optical photon";
		// return;
	}

	//kill photons that enter PMT
	G4bool pmt = volume->GetName() == "logicTopCap";

	if(isPhot && pmt){
		G4cout << "Optical photon detected in PMT!" << G4endl;
		const double t_drift_us = post->GetGlobalTime() / us; // Δt since creation
    	const int tid = track->GetTrackID();

    	// man->FillNtupleDColumn(3, 0, t_drift_us);
   		// man->FillNtupleIColumn(3, 1, track->GetTrackID());
		// man->AddNtupleRow(3);
		track->SetTrackStatus(fStopAndKill);
	}

	if (!std::isfinite(Efield) || Efield < 0.0) return;

	// ~~~~~~~~~~~~~~~~~~~~~~Drift velocity calculation (with diffusion)~~~~~~~~~~~~~~~~~~~~~~

	Efield = 100; //volt/cm constant field for testing
	G4double v_drift = nCalc.SetDriftVelocity(temp, density, Efield, pressure); //average drift velocity (mm/us) baseed on detector parameters
	G4double dt = stepLength / v_drift; //time taken for step
	// G4cout << "Step length: " << stepLength << " mm" << G4endl;

	//calculating transverse and longitudinal diffusion coefficients
	G4double D_Tz = nCalc.GetDiffTran_Liquid(Efield, false, temp, pressure, density, 54) * pow(10,-4); // converts from cm2/s to mm2/us
	G4double D_Tx = nCalc.GetDiffTran_Liquid(Efield, false, temp, pressure, density, 54) * pow(10,-4); // converts from cm2/s to mm2/us
	G4double D_L = nCalc.GetDiffLong_Liquid(Efield, false, temp, pressure, density, 54, 0) * pow(10, -4); // converts from cm2/s mm2/us 
	
	G4double sigma_L = sqrt(D_L * dt);
    G4double sigma_Tz = sqrt(D_Tz * dt);
	G4double sigma_Tx = sqrt(D_Tx * dt);

	G4double diff_x = G4RandGauss::shoot(0., sigma_Tx); //transverse diffusion in x
	G4double diff_y = G4RandGauss::shoot(0., sigma_L); //longitudinal diffusion in y
	G4double diff_z = G4RandGauss::shoot(0., sigma_Tz); //transverse diffusion in z
	G4ThreeVector diffusion(diff_x/dt, diff_y/dt, diff_z/dt); //total diffusion for 1 step
	// G4cout << "Diffusion Vector: " << diffusion << " mm" << G4endl;
	G4ThreeVector v_drift_dir = (G4ThreeVector(0., v_drift, 0.) + diffusion).unit(); //drift in +y direction with diffusion
	

	//Nest Part

  	if (volume->GetName() == "logicLXe" || volume->GetName() == "logicGXe")
	{

		if (particleDefinition == G4Electron::ElectronDefinition() || particleDefinition->GetParticleName() == "e-")
		{
			if ((volume->GetName() == "logicGXe" || (pos.y() <= -22.*mm && pos.y() >= -44.*mm)) && taggedTracksS2.count(trackID) <= 1)
			{
				nPhotons = photPerE;
				nElectrons = 0;
				S2Event = true;
				nS2Events++;
				G4cout << "--------------initializing S2 event--------------" << G4endl;
			}

			taggedTracksS2.insert(trackID);
		}

		if(volume->GetName() == "logicLXe" && energyDeposit > 0 /*&& pd != G4Gamma::GammaDefinition()*/ && pd != G4Electron::ElectronDefinition() && pd != G4OpticalPhoton::OpticalPhotonDefinition())
		{
			// Create NEST object
			taggedTracksS1.insert(trackID);
			S1Event = true;
			G4cout << "~~~~~~~~~~~~~~~initializing S1 event~~~~~~~~~~~~~~~" << G4endl;
			G4cout << "Particle causing S1: " << particleDefinition->GetParticleName() << G4endl;
			// G4cout << "Drift Velocity: " << v_drift << " mm/us" << G4endl;
			y_S1 = pos.y()/mm;
			// man->FillNtupleIColumn(3, 1, y_S1);

			double Efield = nestDetector->get_ElectricField(pos.x(), pos.y(), pos.z());
			NEST::YieldResult yields = nestCalc->GetYields(recoilType, edep*10, density, Efield,          // drift field
			131,                     // A
			54                           // Z
			);
			nPhotons = safeRound(yields.PhotonYield*edep);
			nElectrons = safeRound(yields.ElectronYield*edep);
			// if(nPhotons > 1000) {
			// 	nPhotons = 1000;
			// }
			// if(nElectrons > 100) {
			// 	nElectrons = 100;
			// }
			// if(nElectrons > 0 && nPhotons == 0) {
			// 	nPhotons = 1; // ensure at least 1 photon if electrons are produced, to avoid zero-photon events
			// }
			// G4cout << "Number of yield electrons: " << nElectrons << G4endl;
			// G4cout << "Number of yield photons: " << nPhotons << G4endl;
			// G4cout << yields.PhotonYield << G4endl;
			// G4cout << yields.ElectronYield << G4endl;
			yieldElectrons += nElectrons;
			yieldPhotons += nPhotons;
			S1phot += nPhotons;

			if(edep > 0) {
				G4cout << "position: " << pos << G4endl;
				// G4cout << "Efield: " << Efield << G4endl;
				G4cout << "edep (keV): " << edep << G4endl;
				G4cout << "recoil type: " << recoilType << G4endl;
				G4cout << "Incident Particle: " << incidentParticle << G4endl;
				G4cout << "Particle Energy: " << energy/keV << " keV" << G4endl;
				// G4cout << "Track ID: " << trackID << G4endl;
				G4cout << "   " << G4endl;
				G4cout << "Photon Yield: " << nPhotons << G4endl;
				G4cout << "Electron Yield: " << nElectrons << G4endl;
				G4cout << "Unrounded Photon Yield: " << yields.PhotonYield << G4endl;
				G4cout << "Unrounded Electron Yield: " << yields.ElectronYield << G4endl;
			}
			
		}

		G4ParticleDefinition* photonDef = G4OpticalPhoton::OpticalPhotonDefinition();
    	G4ParticleDefinition* eDef = G4Electron::ElectronDefinition(); 
			
		G4TrackVector* secondaries = new G4TrackVector();


    //Spawn optical photons
    	for (int i = 0; i < nPhotons; ++i) 
		{
			
			G4ThreeVector dir = RandomUnitVector();
			G4double photonEnergy = SampleLXePhotonEnergy_GaussEnergy();  

			G4double t_create = t0; // simplest model: all photons created "now"

  			// Store the time PER PHOTON at creation:
  			if (S1Event) fEventAction->AddS1PhotonTime(t_create);
  			if (S2Event) fEventAction->AddS2PhotonTime(t_create);

			G4DynamicParticle* dynPart = new G4DynamicParticle(photonDef, dir, photonEnergy);
			G4ThreeVector perp = dir.orthogonal();
			G4double phi = CLHEP::twopi * G4UniformRand();
			G4ThreeVector pol = perp.rotate(dir, phi).unit();
			dynPart->SetPolarization(pol);

			G4Track* newTrack = new G4Track(dynPart, track->GetGlobalTime(), pos);
			newTrack->SetTouchableHandle(track->GetTouchableHandle());
			newTrack->SetParentID(track->GetTrackID());
			secondaries->push_back(newTrack);
    	}

    //Spawn electrons

	G4double mass_e = G4Electron::ElectronDefinition()->GetPDGMass() /kg ; // /kg 1.782662 * pow(10, -30)
    	for (int i = 0; i < nElectrons; ++i) 
		{
			// G4cout << "Drift Velocity: " << v_drift << " mm/us" << G4endl;

			 //convert from MeV/c^2 to kg )

			G4double t_create = t0;

        	// G4double electronEnergy = 0.1*eV;

			v_drift = nCalc.SetDriftVelocity(temp, density, Efield, pressure); //average drift velocity (mm/us) baseed on detector parameters
			G4double dt = stepLength / v_drift; //time taken for step
			// G4cout << "Step length: " << stepLength << " mm" << G4endl;

			//calculating transverse and longitudinal diffusion coefficients
			G4double D_Tz = nCalc.GetDiffTran_Liquid(Efield, false, temp, pressure, density, 54) * pow(10,-4); // converts from cm2/s to mm2/us
			G4double D_Tx = nCalc.GetDiffTran_Liquid(Efield, false, temp, pressure, density, 54) * pow(10,-4); // converts from cm2/s to mm2/us
			G4double D_L = nCalc.GetDiffLong_Liquid(Efield, false, temp, pressure, density, 54, 0) * pow(10, -4); // converts from cm2/s mm2/us 
	
			G4double sigma_L = sqrt(D_L * dt);
    		G4double sigma_Tz = sqrt(D_Tz * dt);
			G4double sigma_Tx = sqrt(D_Tx * dt);

			G4double diff_x = G4RandGauss::shoot(0., sigma_Tx); //transverse diffusion in x
			G4double diff_y = G4RandGauss::shoot(0., sigma_L); //longitudinal diffusion in y
			G4double diff_z = G4RandGauss::shoot(0., sigma_Tz); //transverse diffusion in z
			diffusion = G4ThreeVector(diff_x/dt, diff_y/dt, diff_z/dt); //total diffusion for 1 step
			// G4cout << "Diffusion Vector: " << diffusion << " mm" << G4endl;
			G4ThreeVector v_drift_dir = (G4ThreeVector(0., v_drift, 0.) + diffusion).unit(); //drift in +y direction with diffusion
			
			G4double v_drift_m_s =v_drift * 1000; // convert from mm/us to m/s 
			G4double electronEnergy = 0.5 * mass_e * v_drift_m_s * v_drift_m_s * joule;
			
        	G4DynamicParticle* dynPart = new G4DynamicParticle(eDef, v_drift_dir, electronEnergy);
        	G4Track* newTrack = new G4Track(dynPart, track->GetGlobalTime(), pos);
			newTrack->SetUserInformation(new DriftElectronInfo(true));
        	newTrack->SetTouchableHandle(track->GetTouchableHandle());
        	newTrack->SetParentID(track->GetTrackID());
			DriftTrackIDs.insert(newTrack->GetTrackID());

        	secondaries->push_back(newTrack);

			createdElectrons = createdElectrons + 1;

			t_S1 = step->GetPreStepPoint()->GetGlobalTime();
			
			// times_LXe.push_back(t_S1);
			fEventAction->AddTimesLXe(t_S1);
			//cout << "------------------------Number of Created Electrons: " << createdElectrons << G4endl;

			fEventAction->RecordElectronCreate(newTrack->GetTrackID(), t_create);

    	}

		if(!(pd == G4Electron::ElectronDefinition())) {
			v_drift = 0;
			diffusion = G4ThreeVector(0., 0., 0.);
		}

		if(pd == G4Electron::ElectronDefinition() && volume->GetName() == "logicLXe"){

			if(std::isfinite(diffusion.x()) && std::isfinite(diffusion.y()) && std::isfinite(diffusion.z())) {
				G4ThreeVector eDir = step->GetPreStepPoint()->GetMomentumDirection();
			G4ThreeVector newDiff = (diffusion + eDir).unit();
			track->SetMomentumDirection(newDiff);
			track->SetKineticEnergy(v_drift*1000 * mass_e * kg/1.782662 * pow(10, -30) * v_drift*1000 / 2 * joule); // set kinetic energy based on drift velocity
			}
			else {
				//do nothing
			}
			// G4cout << "Scattering drift electron!" << G4endl;
			// G4cout << "New direction: " << newDiff << G4endl;
		}

    	if (!secondaries->empty()) 
		{
			G4TrackVector* trackSecondaries = const_cast<G4Step*>(step)->GetfSecondary();
        	trackSecondaries->insert(trackSecondaries->end(), secondaries->begin(), secondaries->end());
			//cout << "done creating secondaries" << G4endl;
    	}

		// Record drift velocity data
		if (driftElectron && postVol && post->GetStepStatus() == fGeomBoundary && (pmt || postVol->GetName() == "logicGXe")) {
			G4double t_enterGXe = post->GetGlobalTime(); // Δt since creation
			// times_GXe.push_back(t_enterGXe);
			fEventAction->AddTimesGXe(t_enterGXe);
			// G4double t_drift_us = t_enterGXe - t_S1;
    		// G4int tid = track->GetTrackID();


    		// man->FillNtupleDColumn(3, 0, t_drift_us);
			// man->FillNtupleIColumn(3, 1, y_S1);
			// s
		}

		if(S2Event)
		{
			// nS2Events++;
			totalS2Photons+=(nPhotons - S1phot);
		}
		else
		{
			nS1Events++;
			totalS1Photons+=S1phot;
		}

		nPhotons = 0;
		S1phot = 0;
		
		if(S2Event)
		{
			// track->SetTrackStatus(fStopAndKill);
			ClearStagnationData(id);
		}
    	delete secondaries;

    }

	auto prePoint  = step->GetPreStepPoint();
	auto postPoint = step->GetPostStepPoint();
	if (!prePoint || !postPoint) return;

	auto preVolPhys  = prePoint->GetTouchableHandle()->GetVolume();
	auto postVolPhys = postPoint->GetTouchableHandle()->GetVolume();
	if (!preVolPhys || !postVolPhys) return;

	auto preLV  = preVolPhys->GetLogicalVolume();
	auto postLV = postVolPhys->GetLogicalVolume();
	if (!preLV || !postLV) return;

	const bool isBoundary = (postPoint->GetStepStatus() == fGeomBoundary);

	if (pd == G4Electron::ElectronDefinition() && isBoundary && preLV->GetName() == "logicLXe" && postLV->GetName() != "logicLXe")     // or postLV != logicLXe if you prefer
	{
  		G4int tid = track->GetTrackID();
  		G4double t_exit = postPoint->GetGlobalTime();

  		// store only the first exit time for this electron
  		fEventAction->RecordElectronExitLXe(tid, t_exit);
		G4cout << "EXIT LXe->GXe tid=" << track->GetTrackID() << " t(us)=" << postPoint->GetGlobalTime()/us << G4endl;
	}	

	// if(pd == G4Electron::ElectronDefinition()) {
	// 	G4String procName = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();

	// 	MyRunAction::dataFile
    //     << eventID   << "  "
    //     << trackID   << "  "
    //     << stepNo    << "  "
    //     << procName  << "  "
    //     << pos.x()/CLHEP::mm << "  "
    //     << pos.y()/CLHEP::mm << "  "
    //     << pos.z()/CLHEP::mm << "  "
    //     << stepLength/CLHEP::mm << "  "
    //     << "\n";
	// }


	previousEnergy[id] = energy;

	//Kill particles that escape to make viwewer look better
	if (pos.y() > 0.*mm || pos.y() <= -100.*mm)
	{
		// volume->GetName() == "logicWorld"
		track->SetTrackStatus(fStopAndKill);
		// G4cout << "killing particle that escaped TPC" << G4endl;
	}

	//fEventAction->AddEdep(edep);
}