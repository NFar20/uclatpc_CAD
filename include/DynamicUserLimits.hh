#ifndef DYNAMICUSERLIMITS_HH
#define DYNAMICUSERLIMITS_HH

#include "G4UserLimits.hh"
#include "globals.hh"
#include "G4Track.hh"
#include "G4Electron.hh"
#include "G4ParticleTypes.hh"
#include <cmath>

extern double gainArea;

class DynamicUserLimits : public G4UserLimits {
public:
    DynamicUserLimits(G4double farStep, G4double nearStep)
        : G4UserLimits(farStep),
          fFarStep(farStep),
          fNearStep(nearStep)
          {}

    virtual G4double GetMaxAllowedStep(const G4Track& track) override
    {
        G4double step = fFarStep;
        if (track.GetDefinition() == G4Electron::Definition())
        {
            G4double x = track.GetPosition().x();
            G4double y = track.GetPosition().y();
            G4double dist = std::sqrt(x*x + y*y);

            step = 0.25*(dist - gainArea);  //0.5*(dist - gainArea);
        }
        if (step <= fNearStep)
        {
            step = fNearStep;
        }
        if (step >= fFarStep)
        {
            step = fFarStep;
        }

        return step;
    }

private:
    G4double fFarStep;
    G4double fNearStep;
};

#endif