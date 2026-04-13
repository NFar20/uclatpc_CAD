// #include "stacking.hh"

// #include "event.hh"

// #include "G4OpticalPhoton.hh"
// #include "G4Track.hh"
// #include "G4VProcess.hh"

// //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// LXeStackingAction::LXeStackingAction(MyEventAction* ea)
//   : fEventAction(ea)
// {}

// //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// LXeStackingAction::~LXeStackingAction() {}

// //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// G4ClassificationOfNewTrack LXeStackingAction::ClassifyNewTrack(
//   const G4Track* aTrack)
// {
//   // Count what process generated the optical photons
//   if(aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
//   {
//     // particle is optical photon
//     if(aTrack->GetParentID() > 0)
//     {
//       // particle is secondary
//       if(aTrack->GetCreatorProcess()->GetProcessName() == "S1")
//         fEventAction->IncPhotonCount_Scint();
//       else if(aTrack->GetCreatorProcess()->GetProcessName() == "Cerenkov")
//         fEventAction->IncPhotonCount_Ceren();
//     }
//   }
//   savedManager = stackManager;
//   if (aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) {
//      return fWaiting;
//   }
//   return fUrgent;
// }