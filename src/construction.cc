#include "construction.hh"

MyDetectorConstruction::MyDetectorConstruction()
{}

MyDetectorConstruction::~MyDetectorConstruction()
{}

G4VPhysicalVolume *MyDetectorConstruction::Construct()
{
	G4NistManager *nist = G4NistManager::Instance();
	
	G4Material *SiO2 = new G4Material("SiO2", 2.201*g/cm3, 2);
	SiO2->AddElement(nist->FindOrBuildElement("Si"), 1);
	SiO2->AddElement(nist->FindOrBuildElement("O"), 2);
	
	G4Material *H2O = new G4Material("H2O", 1.000*g/cm3, 2);
	H2O->AddElement(nist->FindOrBuildElement("H"), 2);
	H2O->AddElement(nist->FindOrBuildElement("O"), 1);
	
	G4Element *C = nist->FindOrBuildElement("C");
	G4Element *F = nist->FindOrBuildElement("F");
	
	G4Material *PTFE = new G4Material("PTFE", 2.2*g/cm3, 2);
	PTFE->AddElement(C, 1);
	PTFE->AddElement(F, 2);
	
	G4Element *Fe = nist->FindOrBuildElement("Fe");
	G4Element *Cr = nist->FindOrBuildElement("Cr");
	G4Element *Ni = nist->FindOrBuildElement("Ni");
	
	G4Material *StainlessSteel = new G4Material("StainlessSteel", 7.93*g/cm3, 3);
	StainlessSteel->AddElement(Fe, 74*perCent);
	StainlessSteel->AddElement(Cr, 18*perCent);
	StainlessSteel->AddElement(Ni, 8*perCent);
	
	G4Material *LXe = nist->FindOrBuildMaterial("G4_lXe");
	
	G4MaterialPropertiesTable *LXeMPT = new G4MaterialPropertiesTable();
	G4double LXeRIndex[2] = {1.3, 1.3};
	G4double photonEnergy[2] = {1.55*eV, 10.*eV};
	LXeMPT->AddProperty("RINDEX", photonEnergy, LXeRIndex, 2);
	
	G4double scintPhotonEnergy[2] = {7. * eV, 7.5 * eV};
	G4double scintYield[2] = {1.0, 1.0};
	
	//LXeMPT->AddProperty("SCINTILLATIONCOMPONENT1", scintPhotonEnergy, scintYield, 2);
	//LXeMPT->AddConstProperty("SCINTILLATIONYIELD", 0);
	//LXeMPT->AddConstProperty("RESOLUTIONSCALE", 1.);
	//LXeMPT->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 27.*ns, true);
	//LXeMPT->AddConstProperty("SCINTILLATIONYIELD1", 1.);
	
	LXe->SetMaterialPropertiesTable(LXeMPT);

	G4MaterialPropertiesTable *HDPEmpt = new G4MaterialPropertiesTable();
	G4double HDPERIndex[2] = {1.52, 1.52};
	HDPEmpt->AddProperty("RINDEX", photonEnergy, HDPERIndex, 2);

	G4MaterialPropertiesTable *steelMPT = new G4MaterialPropertiesTable();
	G4double steelRIndex[2] = {1.3, 1.3}; //find actual values
	steelMPT->AddProperty("RINDEX", photonEnergy, steelRIndex, 2);

	StainlessSteel->SetMaterialPropertiesTable(steelMPT);
	
	G4Material *GXe = new G4Material("GXe", 0.0059*g/cm3, 1);
	GXe->AddElement(nist->FindOrBuildElement("Xe"), 1);
	
	G4Isotope *isoSe72 = new G4Isotope("isoSe72", 34, 72, 71.9221*g/mole);
	G4Element *elSe72 = new G4Element("elSe72", "Se72", 1);
	elSe72->AddIsotope(isoSe72, 100*perCent);
	G4Material *GSe72 = new G4Material("GSe72", 0.001*g/cm3, 1, kStateGas, 500*kelvin, 1.*atmosphere);
	GSe72->AddElement(elSe72, 1);
	
	G4Isotope *Tritium = new G4Isotope("Tritium", 1, 3, 3.016*g/mole);
	G4Element *T = new G4Element("T", "H3", 1);
	T->AddIsotope(Tritium, 100*perCent);
	G4Material *GCT4 = new G4Material("GCT4", 0.000656*g/cm3, 2);
	GCT4->AddElement(C, 1);
	GCT4->AddElement(T, 4);
	
	G4Isotope *isoRn220 = new G4Isotope("isoRn220", 86, 220, 211*g/mole);
	G4Element *elRn220 = new G4Element("Rn220", "Rn220", 1);
	elRn220->AddIsotope(isoRn220, 100*perCent);
	G4Material *GRn220 = new G4Material("Rn220", 0.00973*g/cm3, 1);
	GRn220->AddElement(elRn220, 1);
	
	G4Material *HDPE = new G4Material("HDPE", 0.94*g/cm3, 2);
	G4Element *H = nist->FindOrBuildElement("H");
	HDPE->AddElement(H, 2);
	HDPE->AddElement(C, 1);

	HDPE->SetMaterialPropertiesTable(HDPEmpt);
	
	G4VisAttributes *defaultVisAttributes = new G4VisAttributes();
	G4Color silver(192., 192., 192., 0.3);
	defaultVisAttributes->SetColor(silver);
	defaultVisAttributes->SetForceWireframe(false);
	defaultVisAttributes->SetForceSolid(true);
	
	G4VisAttributes *HDPEVisAttributes = new G4VisAttributes();
	G4Color cyan(0., 255., 255., 0.6);
	HDPEVisAttributes->SetColor(cyan);
	HDPEVisAttributes->SetForceWireframe(false);
	HDPEVisAttributes->SetForceSolid(true);
	
	G4VisAttributes *PMTVisAttributes = new G4VisAttributes();
	G4Color red(1., 0., 0.);
	PMTVisAttributes->SetColor(red);
	PMTVisAttributes->SetForceWireframe(false);
	PMTVisAttributes->SetForceSolid(true);
	
	G4VisAttributes *blueVisAttributes = new G4VisAttributes();
	G4Color blue(0., 0., 1., 0.6);
	blueVisAttributes->SetColor(blue);
	blueVisAttributes->SetForceWireframe(false);
	blueVisAttributes->SetForceSolid(true);
	
	G4VisAttributes *yellowVisAttributes = new G4VisAttributes();
	G4Color yellow(255., 255., 0., 0.6);
	yellowVisAttributes->SetColor(yellow);
	yellowVisAttributes->SetForceWireframe(false);
	yellowVisAttributes->SetForceSolid(true);
	
	G4VisAttributes *greenVisAttributes = new G4VisAttributes();
	G4Color green(0., 1., 0., 0.6);
	greenVisAttributes->SetColor(green);
	greenVisAttributes->SetForceWireframe(false);
	greenVisAttributes->SetForceSolid(true);
	
	G4VisAttributes *whiteVisAttributes = new G4VisAttributes();
	G4Color white(255., 255., 255., 0.8);
	whiteVisAttributes->SetColor(white);
	whiteVisAttributes->SetForceWireframe(false);
	whiteVisAttributes->SetForceSolid(true);
	
	G4VisAttributes *purpleVisAttributes = new G4VisAttributes();
	G4Color purple(255., 0., 255., 0.3);
	purpleVisAttributes->SetColor(purple);
	purpleVisAttributes->SetForceWireframe(false);
	purpleVisAttributes->SetForceSolid(true);
	
	G4VisAttributes *invisible = new G4VisAttributes();
	invisible->SetVisibility(false);
	
	
	G4double energy[2] = {1.239841939*eV/0.9, 1.239841939*eV/0.2}; //blue and red light
	G4double rindexAerogel[2] = {1.1, 1.1};
	G4double rindexWorld[2] = {1.0, 1.0};
	
	G4Material *worldMat = nist->FindOrBuildMaterial("G4_AIR");
	
	G4MaterialPropertiesTable *mptWorld = new G4MaterialPropertiesTable();
	mptWorld->AddProperty("RINDEX", energy, rindexWorld, 2);
	
	worldMat->SetMaterialPropertiesTable(mptWorld);
	
	G4Box *solidWorld = new G4Box("solidWorld", 0.3*m, 0.3*m, 0.3*m);
	
	G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
	
	G4VPhysicalVolume *physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, true);
	
	//importing CAD files
	
	auto topCap = CADMesh::TessellatedMesh::FromSTL("../UCLA TPC Parts/ATopPMTCap.stl");
	
	auto topPMT = CADMesh::TessellatedMesh::FromSTL("../UCLA TPC Parts/ATopPMT.stl");
	
	auto electrodeMesh = CADMesh::TessellatedMesh::FromSTL("../UCLA TPC Parts/AElectrodeMesh.stl");
	
	auto steelRing = CADMesh::TessellatedMesh::FromSTL("../UCLA TPC Parts/ASteelRing.stl");
	
	auto electrodeSeparationRing = CADMesh::TessellatedMesh::FromSTL("../UCLA TPC Parts/AElectrodeSeparationRing.stl");
	
	auto PESheeth = CADMesh::TessellatedMesh::FromSTL("../UCLA TPC Parts/APolyethyleneSheeth.stl");
	
	auto bottomPMT = CADMesh::TessellatedMesh::FromSTL("../UCLA TPC Parts/ABottomPMT.stl");
	
	auto bottomPMTCap = CADMesh::TessellatedMesh::FromSTL("../UCLA TPC Parts/ABottomPMTCap.stl");
	
	G4RotationMatrix *meshRot = new G4RotationMatrix();
	meshRot->rotateX(-90.*deg);
	
	G4RotationMatrix *meshRot180 = new G4RotationMatrix();
	meshRot180->rotateX(180.*deg);
	
	G4VSolid *solidTopCap = topCap->GetSolid();
	logicTopCap = new G4LogicalVolume(solidTopCap, PTFE, "logicTopCap");
	logicTopCap->SetVisAttributes(defaultVisAttributes);
	
	G4VPhysicalVolume *physTopCap = new G4PVPlacement(meshRot, G4ThreeVector(), logicTopCap, "physTopCap", logicWorld, false, 0, true);
	
	G4VSolid *solidTopPMT = topPMT->GetSolid();
	logicPMT = new G4LogicalVolume(solidTopPMT, PTFE, "logicPMT");
	logicPMT->SetVisAttributes(PMTVisAttributes);
	
	G4VPhysicalVolume *physPMT0 = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 0.), logicPMT, "physPMT", logicWorld, false, 0, true);
	G4VPhysicalVolume *physPMT1 = new G4PVPlacement(meshRot, G4ThreeVector(-29.7*mm, 0., 0.), logicPMT, "physPMT", logicWorld, false, 1, true);
	G4VPhysicalVolume *physPMT2 = new G4PVPlacement(meshRot, G4ThreeVector(29.7*mm, 0., 0.), logicPMT, "physPMT", logicWorld, false, 2, true);
	G4VPhysicalVolume *physPMT3 = new G4PVPlacement(meshRot, G4ThreeVector(-14.85*mm, -29.7*mm, 0.), logicPMT, "physPMT", logicWorld, false, 3, true);
	G4VPhysicalVolume *physPMT4 = new G4PVPlacement(meshRot, G4ThreeVector(14.85*mm, -29.7*mm, 0.), logicPMT, "physPMT", logicWorld, false, 4, true);
	G4VPhysicalVolume *physPMT5 = new G4PVPlacement(meshRot, G4ThreeVector(-14.85*mm, 29.7*mm, 0.), logicPMT, "physPMT", logicWorld, false, 5, true);
	G4VPhysicalVolume *physPMT6 = new G4PVPlacement(meshRot, G4ThreeVector(14.85*mm, 29.7*mm, 0.), logicPMT, "physPMT", logicWorld, false, 6, true);
	
	G4VSolid *solidElectrodeMesh = electrodeMesh->GetSolid();
	G4LogicalVolume *logicElectrodeMesh = new G4LogicalVolume(solidElectrodeMesh, StainlessSteel, "logicElectrodeMesh");
	logicElectrodeMesh->SetVisAttributes(whiteVisAttributes);
	
	G4VPhysicalVolume *physElectrodeMesh0 = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 32.*mm), logicElectrodeMesh, "physElectrodeMesh", logicWorld, false, 0, true);
	G4VPhysicalVolume *physElectrodeMesh1 = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 50.2*mm), logicElectrodeMesh, "physElectrodeMesh", logicWorld, false, 1, true);
	G4VPhysicalVolume *physElectrodeMesh2 = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 110.4*mm), logicElectrodeMesh, "physElectrodeMesh", logicWorld, false, 2, true);
	G4VPhysicalVolume *physElectrodeMesh3 = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 124.6*mm), logicElectrodeMesh, "physElectrodeMesh", logicWorld, false, 3, true);
	
	G4VSolid *solidSteelRing = steelRing->GetSolid();
	G4LogicalVolume *logicSteelRing = new G4LogicalVolume(solidSteelRing, StainlessSteel, "logicSteelRing");
	logicSteelRing->SetVisAttributes(blueVisAttributes);
	
	G4VPhysicalVolume *physSteelRing0 = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 32.2*mm), logicSteelRing, "physSteelRing", logicWorld, false, 0, true);
	
	G4VSolid *solidElectrodeSeparationRing = electrodeSeparationRing->GetSolid();
	G4LogicalVolume *logicElectrodeSeparationRing = new G4LogicalVolume(solidElectrodeSeparationRing, StainlessSteel, "logicElectrodeSeparationRing");
	logicElectrodeSeparationRing->SetVisAttributes(yellowVisAttributes);
	
	G4VPhysicalVolume *physTopElectrodeSeparationRing = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 36.2*mm), logicElectrodeSeparationRing, "physElectrodeSeparationRing", logicWorld, false, 0, true);
	
	G4VPhysicalVolume *physSteelRing1 = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 46.2*mm), logicSteelRing, "physSteelRing", logicWorld, false, 1, true);
	
	G4VSolid *solidPESheeth = PESheeth->GetSolid();
	G4LogicalVolume *logicPESheeth = new G4LogicalVolume(solidPESheeth, HDPE, "logicPESheeth");
	logicPESheeth->SetVisAttributes(invisible); //HDPEVisAttributes
	G4VPhysicalVolume *physPESheeth = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 51.4*mm), logicPESheeth, "physPESheeth", logicWorld, false, 0, true);
	
	G4VPhysicalVolume *physSteelRing2 = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 106.4*mm), logicSteelRing, "physSteelRing", logicWorld, false, 2, true);
	
	G4VPhysicalVolume *physBottomElectrodeSeparationRing = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 110.6*mm), logicElectrodeSeparationRing, "physElectrodeSeparationRing", logicWorld, false, 1, true);
	
	G4VPhysicalVolume *physSteelRing3 = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 120.6*mm), logicSteelRing, "physSteelRing", logicWorld, false, 3, true);
	
	G4VSolid *solidBottomCap = bottomPMTCap->GetSolid();
	G4LogicalVolume *logicBottomCap = new G4LogicalVolume(solidBottomCap, PTFE, "logicBottomCap");
	logicBottomCap->SetVisAttributes(defaultVisAttributes);
	
	//G4VPhysicalVolume *physBottomCap = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 104.8*mm), logicBottomCap, "physBottomCap", logicWorld, false, 0, true);
	
	G4VSolid *solidBottomPMTCap = bottomPMTCap->GetSolid();
	G4LogicalVolume *logicBottomPMTCap = new G4LogicalVolume(solidBottomPMTCap, PTFE, "logicBottomPMTCap");
	logicBottomPMTCap->SetVisAttributes(defaultVisAttributes);
	
	G4VPhysicalVolume *physBottomPMTCap = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 104.8*mm), logicBottomPMTCap, "physBottomPMTCap", logicWorld, false, 0, true);
	
	G4VSolid *solidBottomPMT = bottomPMT->GetSolid();
	logicBottomPMT = new G4LogicalVolume(solidBottomPMT, PTFE, "logicInnerBottomPMT");
	logicBottomPMT->SetVisAttributes(PMTVisAttributes);
	
	G4VPhysicalVolume *physBottomPMT = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 125.8*mm), logicBottomPMT, "physBottomPMT", logicWorld, false, 0, true);
	
	
	G4Tubs *solidLXe = new G4Tubs("solidLXe", 0.*mm, 46.4213333*mm, 28.*mm, 0.*deg, 360.*deg);
	logicLXe = new G4LogicalVolume(solidLXe, LXe, "logicLXe");
	logicLXe->SetVisAttributes(invisible); //purpleVisAttributes
	
	G4VPhysicalVolume *physLXe = new G4PVPlacement(0, G4ThreeVector(0., 0., 78.4*mm), logicLXe, "physLXe", logicWorld, false, 0, true);
	
	fScoringVolume = logicLXe;
	
	//constructing electric field
	G4ThreeVector electricFieldVector(0., 0., 100.*kilovolt/um);
	
	electricField = new ElectricField();
	//electricField = new G4UniformElectricField(electricFieldVector);
	
	equation = new G4EqMagElectricField(electricField);
	
	G4int nvar = 8;
	
	auto pStepper = new G4DormandPrince745(equation, nvar);
	
	auto globalFieldManager = G4TransportationManager::GetTransportationManager()->GetFieldManager();
	
	auto localFieldManager = new G4FieldManager(electricField);
	localFieldManager->SetDetectorField(electricField);
	
	G4double minStep = 0.001*mm;
	auto pIntegrationDriver = new G4IntegrationDriver<G4DormandPrince745>(minStep, pStepper, nvar);
	
	chordFinder = new G4ChordFinder(pIntegrationDriver);
	globalFieldManager->SetChordFinder(chordFinder);
	localFieldManager->SetChordFinder(chordFinder);
	
	logicLXe->SetFieldManager(localFieldManager, true);
	
	
	return physWorld;
}

void MyDetectorConstruction::ConstructSDandField()
{
	G4SDManager *sdManager = G4SDManager::GetSDMpointer();
	
	sensPMT = new MySensitiveDetector("sensPMT");
	sdManager->AddNewDetector(sensPMT);
	
	logicPMT->SetSensitiveDetector(sensPMT);
	//logicBottomPMT->SetSensitiveDetector(sensPMT);
}
