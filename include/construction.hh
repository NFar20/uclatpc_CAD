#ifndef CONSTRUCTION_HH
#define CONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4GenericMessenger.hh"
#include "G4Isotope.hh"
//#include "G4SubtractionSolid.hh"

#include "G4VisAttributes.hh"
#include"G4UniformElectricField.hh"
#include "G4ElectricField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4EqMagElectricField.hh"
#include "G4DormandPrince745.hh"
#include "G4ChordFinder.hh"

#include "detector.hh"
#include "electricField.hh"

#include "CADMesh.hh"

class MyDetectorConstruction : public G4VUserDetectorConstruction
{
public:
	MyDetectorConstruction();
	~MyDetectorConstruction();
	
	G4LogicalVolume *GetScoringVolume() const {return fScoringVolume;}
	
	G4LogicalVolume *logicDetector, *logicPMT, *logicTopCap, *logicLXe, *logicBottomPMT;
	
	MySensitiveDetector *sensPMT;
	
	G4ElectroMagneticField *electricField, *gasElectricField;
	G4EqMagElectricField *equation, *gasEquation;
	G4ChordFinder *chordFinder;
	
	virtual G4VPhysicalVolume *Construct();
	
private:
	
	G4LogicalVolume *fScoringVolume;
	
	virtual void ConstructSDandField();
};

#endif
