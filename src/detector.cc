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
	G4bool gamma = track->GetDefinition() == particleTable->FindParticle("gamma");
	
	if(opticalPhoton || gamma)
	{
		track->SetTrackStatus(fStopAndKill); //kills photon after detection
		
		G4StepPoint *preStepPoint = aStep->GetPreStepPoint(); //when photon enters detector
		G4StepPoint *postStepPoint = aStep->GetPostStepPoint(); //when photon exits detector
		
		//for charged particles, there is a pre and post step point after every stepping process
		
		G4ThreeVector posPhoton = preStepPoint->GetPosition();
		
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
		
		G4int type = -1;
		if(opticalPhoton)
		{
			type = 0;
		}
		else if(gamma)
		{
			type = 1;
		}
		
		G4int parentID = track->GetParentID();
	
		G4AnalysisManager *man = G4AnalysisManager::Instance();
		
		man->FillNtupleIColumn(0, 0, evt);
		man->FillNtupleIColumn(0, 1, type);
		man->FillNtupleIColumn(0, 2, parentID);
		man->FillNtupleDColumn(0, 3, posDetector[0]);
		man->FillNtupleDColumn(0, 4, posDetector[1]);
		man->FillNtupleDColumn(0, 5, posDetector[2]);
		man->AddNtupleRow(0);
		
	}
	return true;
}
