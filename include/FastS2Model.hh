#ifndef FASTS2MODEL_HH
#define FASTS2MODEL_HH

#include "G4VFastSimulationModel.hh"
#include "G4ThreeVector.hh"
#include "G4FastTrack.hh"
#include "G4FastStep.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "nestPart.hh"
#include "DriftElectronInfo.hh"
#include "G4Electron.hh"
#include "G4OpticalPhoton.hh"
#include "G4DynamicParticle.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "NEST.hh"
#include "stepping.hh"
#include "G4SystemOfUnits.hh"
#include "run.hh"
#include "DynamicUserLimits.hh"


class LXeElectronDriftModel : public G4VFastSimulationModel {
public:
    LXeElectronDriftModel(const G4String& modelName, G4Region* envelope, nestPart* nptr)
        : G4VFastSimulationModel(modelName, envelope), nestCalc(nptr) {}

    ~LXeElectronDriftModel() override {}

    G4bool IsApplicable(const G4ParticleDefinition& particle) override {
        return &particle == G4Electron::ElectronDefinition();
    }

    G4bool ModelTrigger(const G4FastTrack& fastTrack) override {
        const DriftElectronInfo* info =
            dynamic_cast<const DriftElectronInfo*>(fastTrack.GetPrimaryTrack()->GetUserInformation());
        return info && info->IsDrift(); 
    }


    void DoIt(const G4FastTrack& fastTrack, G4FastStep& fastStep) override;

private:
    nestPart* nestCalc;
    G4double test;

    bool CheckGainRegion(double x, double y, double z, double Efield) const {
        return (y > -44.*mm && y < -26.*mm && sqrt(x*x + z*z) < 52.5*mm);
    }

    double ElThreshold = 2000.; 
    nestPart* nestDetector;  // pointer to the nestPart detector

};

#endif