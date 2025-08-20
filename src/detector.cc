#include "detector.hh"

MySensitiveDetector::MySensitiveDetector(G4String name) : G4VSensitiveDetector(name)
{}

MySensitiveDetector::~MySensitiveDetector()
{}

G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist)
{
	//G4cout << "ProcessHits called" << G4endl;
	G4Track *track = aStep->GetTrack();
	
	G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
	
	G4bool opticalPhoton = track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition();
	
	zSurface = 50.2*mm;

	if(!opticalPhoton){
		return false;
	}
	else
	{
		track->SetTrackStatus(fStopAndKill); //kills photon after detection
		
		G4StepPoint *preStepPoint = aStep->GetPreStepPoint(); //when photon enters detector
		G4StepPoint *postStepPoint = aStep->GetPostStepPoint(); //when photon exits detector
		
		//for charged particles, there is a pre and post step point after every stepping process
		
		G4ThreeVector posPhoton = preStepPoint->GetPosition();
		const G4double t_global = aStep->GetPreStepPoint()->GetGlobalTime(); // in ns
  		const auto& vtx = track->GetVertexPosition();

		const G4bool isS2 = (vtx.z() < zSurface);
		
		//G4cout << "Photon position: " << posPhoton << G4endl;
		//live photon position
		//won't have above line in reality, we will only have Cerenkov angle
		
		const G4VTouchable *touchable = aStep->GetPreStepPoint()->GetTouchable();
		
		G4int copyNo = touchable->GetCopyNumber();
		
		//G4cout << "Copy number: " << copyNo << G4endl;
		//number of detector that has been hit
		
		G4VPhysicalVolume *physVol = touchable->GetVolume(0);
		posDetector = physVol->GetTranslation();
		
		G4cout << "Detector position: " << posDetector << G4endl;
		//position of detector that has been hit
	
		G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
	
		G4AnalysisManager *man = G4AnalysisManager::Instance();

		man->FillH1(0, t_global);
  		man->FillH1(isS2 ? 2 : 1, t_global);
		
		// man->FillNtupleIColumn(0, 0, evt);
		// man->FillNtupleDColumn(0, 3, posDetector[0]);
		// man->FillNtupleDColumn(0, 4, posDetector[1]);
		// man->FillNtupleDColumn(0, 5, posDetector[2]);
		// man->AddNtupleRow(0);
		
	}
	return true;
}
