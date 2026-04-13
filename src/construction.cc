#include "construction.hh"

nestPart detector;

MyDetectorConstruction::MyDetectorConstruction()
{}

MyDetectorConstruction::~MyDetectorConstruction()
{}

G4VPhysicalVolume *MyDetectorConstruction::Construct()
{
	G4NistManager *nist = G4NistManager::Instance(); //initialize NIST
	G4cout << "NIST Manager initialized." << G4endl;

	//creating materials for simulation and adding properties

	G4Material *PTFE = new G4Material("PTFE", 2.2*g/cm3, 2);
	G4Element *C = nist->FindOrBuildElement("C");
	G4Element *F = nist->FindOrBuildElement("F");
	PTFE->AddElement(C, 1);
	PTFE->AddElement(F, 2);
	
	G4Material *LXe = nist->FindOrBuildMaterial("G4_lXe");
	
	G4MaterialPropertiesTable *LXeMPT = new G4MaterialPropertiesTable();
	G4double LXeRIndex[2] = {1.5, 1.5};
	G4double photonEnergy[2] = {1.55*eV, 10.*eV};
	LXeMPT->AddProperty("RINDEX", photonEnergy, LXeRIndex, 2);

	G4Material *GXe = nist->FindOrBuildMaterial("G4_Xe");

	G4MaterialPropertiesTable *GXeMPT = new G4MaterialPropertiesTable();
	G4double GXeRIndex[2] = {1.0007, 1.0007};
	GXeMPT->AddProperty("RINDEX", photonEnergy, GXeRIndex, 2);

	
	G4double scintPhotonEnergy[2] = {7. * eV, 7.5 * eV};
	G4double scintYield[2] = {1.0, 1.0};
	
	LXeMPT->AddProperty("SCINTILLATIONCOMPONENT1", scintPhotonEnergy, scintYield, 2);
	LXeMPT->AddConstProperty("SCINTILLATIONYIELD", 0);
	LXeMPT->AddConstProperty("RESOLUTIONSCALE", 1.);
	LXeMPT->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 27.*ns, true);
	LXeMPT->AddConstProperty("SCINTILLATIONYIELD1", 1.);
	
	LXe->SetMaterialPropertiesTable(LXeMPT);
	GXe->SetMaterialPropertiesTable(GXeMPT);

	G4MaterialPropertiesTable *HDPEmpt = new G4MaterialPropertiesTable();
	G4double HDPERIndex[2] = {1.52, 1.52};
	HDPEmpt->AddProperty("RINDEX", photonEnergy, HDPERIndex, 2);

	G4MaterialPropertiesTable *PTFEmpt = new G4MaterialPropertiesTable();
	G4double PTFERIndex[2] = {1.5, 1.5};
	PTFEmpt->AddProperty("RINDEX", photonEnergy, PTFERIndex, 2);

	G4MaterialPropertiesTable *steelMPT = new G4MaterialPropertiesTable();
	G4double steelRIndex[2] = {1.3, 1.3}; //find actual values
	steelMPT->AddProperty("RINDEX", photonEnergy, steelRIndex, 2);

		
	G4Material *steel = nist->FindOrBuildMaterial("G4_STAINLESS-STEEL");
	steel->SetMaterialPropertiesTable(steelMPT);
	
	// G4cout << "Steel test" << G4endl;
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
	
	G4Material *polyethylene = nist->FindOrBuildMaterial("G4_POLYETHYLENE");
	polyethylene->SetMaterialPropertiesTable(HDPEmpt);

	//creating different visual attributes
	
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

	G4VisAttributes *PPTVisAttributes = new G4VisAttributes();
	G4Color whiteOpaque(255., 255., 255., 1.);
	PPTVisAttributes->SetColor(whiteOpaque);
	PPTVisAttributes->SetForceWireframe(false);
	PPTVisAttributes->SetForceSolid(true);
	
	//establishing world material and properties
	
	G4double energy[2] = {1.239841939*eV/0.9, 1.239841939*eV/0.2}; //blue and red light
	G4double rindexAerogel[2] = {1.1, 1.1};
	G4double rindexWorld[2] = {1.0, 1.0};
	
	G4Material *worldMat = nist->FindOrBuildMaterial("G4_AIR");
	
	G4MaterialPropertiesTable *mptWorld = new G4MaterialPropertiesTable();
	mptWorld->AddProperty("RINDEX", energy, rindexWorld, 2);
	
	worldMat->SetMaterialPropertiesTable(mptWorld);

	G4Material *testMat = nist->FindOrBuildMaterial("G4_A-150_TISSUE");
	testMat->SetMaterialPropertiesTable(steelMPT);
	
	// G4Box *solidWorld = new G4Box("solidWorld", 0.3*m, 0.3*m, 0.3*m);
	
	// G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
	
	// G4VPhysicalVolume *physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, true);

	// auto TPC = CADMesh::TessellatedMesh::FromSTL("UpdatedCaliX.stl");

	// G4VSolid *SolidTPC = TPC->GetSolid();
	// G4LogicalVolume *logicTPC = new G4LogicalVolume(SolidTPC, StainlessSteel, "logicTPC");
	// logicTPC->SetVisAttributes(defaultVisAttributes);

	// G4VPhysicalVolume *physTPC = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicTPC, "physTPC", logicWorld, false, 0, true);

	G4GDMLParser parser;
  	parser.SetOverlapCheck(true);
  	// parser.Read("../UpdatedCalixWorldVol.gdml");
	// parser.Read("../CaliXGDMLPleaseWork.gdml");
	// parser.Read("../MergedCalixWorldVol.gdml");
	parser.Read("../CaliXFixedMaterials4.gdml");
	

	// G4LogicalVolume *logicWorld = parser.GetVolume("GDMLBox_WorldBox");
	G4VPhysicalVolume *physWorld = parser.GetWorldVolume("WorldVOL");
	G4LogicalVolume *logicWorld = physWorld->GetLogicalVolume();

	//adding visual attributes to each part from gdml file
	logicTopCap = parser.GetVolume("LV_TopCap_PMTs");
	logicTopCap->SetVisAttributes(PPTVisAttributes); //

	G4LogicalVolume *logicTopSpacer = parser.GetVolume("LV_TopSpacer");
	logicTopSpacer->SetVisAttributes(purpleVisAttributes); // 
	// logicTopSpacer->SetMaterial(HDPE);

	G4LogicalVolume *logicESepRing = parser.GetVolume("LV_ESepRing_SteelRings");
	logicESepRing->SetVisAttributes(yellowVisAttributes); //

	G4LogicalVolume *logicPESheeth = parser.GetVolume("LV_Polythelne_Sheeth");
	logicPESheeth->SetVisAttributes(invisible); // HDPEVisAttributes 
	logicPESheeth->SetMaterial(HDPE);

	G4LogicalVolume *logicESepRingNoOpen = parser.GetVolume("LV_ESepRingNoOpen_SteelRings");
	logicESepRingNoOpen->SetVisAttributes(yellowVisAttributes); // 

	G4LogicalVolume *logicBottomPMTCap = parser.GetVolume("LV_Bottom_PMT_Cap");
	logicBottomPMTCap->SetVisAttributes(defaultVisAttributes); // 
	// logicBottomPMTCap->SetMaterial(HDPE);

	G4LogicalVolume *logicBottomPMT = parser.GetVolume("LV_Hamamatsu_R11410_PMT");
	logicBottomPMT->SetVisAttributes(PPTVisAttributes);
	// logicBottomPMT->SetMaterial(HDPE);

	G4LogicalVolume *logicElectrodeMesh0 = parser.GetVolume("LV_Electrode_Mesh");
	logicElectrodeMesh0->SetVisAttributes(whiteVisAttributes); // 
	// logicElectrodeMesh0->SetMaterial(StainlessSteel);

	G4LogicalVolume *logicElectrodeMesh1 = parser.GetVolume("LV_Electrode_Mesh001");
	logicElectrodeMesh1->SetVisAttributes(whiteVisAttributes); // 
	// logicElectrodeMesh1->SetMaterial(StainlessSteel);

	G4LogicalVolume *logicElectrodeMesh2 = parser.GetVolume("LV_Electrode_Mesh002");
	logicElectrodeMesh2->SetVisAttributes(whiteVisAttributes); // 
	// logicElectrodeMesh2->SetMaterial(StainlessSteel);

	G4LogicalVolume *logicElectrodeMesh3 = parser.GetVolume("LV_Electrode_Mesh003");
	logicElectrodeMesh3->SetVisAttributes(whiteVisAttributes); // 
	// logicElectrodeMesh3->SetMaterial(StainlessSteel);
	
	G4RotationMatrix *meshRot = new G4RotationMatrix();
	meshRot->rotateX(-90.*deg);
	// G4RotationMatrix *meshRot180 = new G4RotationMatrix();
	// meshRot180->rotateX(180.*deg);
	// G4RotationMatrix *meshRot270 = new G4RotationMatrix();
	// meshRot270->rotateX(270.*deg);
	
	// //defining logical volumes in simulation (using different parts from gdml file)
	// G4VSolid *solidTopCap = topCap->GetSolid();
	// logicTopCap = new G4LogicalVolume(solidTopCap, PTFE, "logicTopCap");
	
	// G4VPhysicalVolume *physTopCap = new G4PVPlacement(meshRot, G4ThreeVector(), logicTopCap, "physTopCap", logicWorld, false, 0, true);
	
	// G4VSolid *solidTopPMT = topPMT->GetSolid();
	// logicPMT = new G4LogicalVolume(solidTopPMT, PTFE, "logicPMT");
	// logicPMT->SetVisAttributes(PMTVisAttributes);
	
	// G4VPhysicalVolume *physPMT0 = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 3.5), logicPMT, "physPMT", logicWorld, false, 0, true);
	// G4VPhysicalVolume *physPMT1 = new G4PVPlacement(meshRot, G4ThreeVector(-29.7*mm, 0., 3.5), logicPMT, "physPMT", logicWorld, false, 1, true);
	// G4VPhysicalVolume *physPMT2 = new G4PVPlacement(meshRot, G4ThreeVector(29.7*mm, 0., 3.5), logicPMT, "physPMT", logicWorld, false, 2, true);
	// G4VPhysicalVolume *physPMT3 = new G4PVPlacement(meshRot, G4ThreeVector(-14.85*mm, -29.7*mm, 3.5), logicPMT, "physPMT", logicWorld, false, 3, true);
	// G4VPhysicalVolume *physPMT4 = new G4PVPlacement(meshRot, G4ThreeVector(14.85*mm, -29.7*mm, 3.5), logicPMT, "physPMT", logicWorld, false, 4, true);
	// G4VPhysicalVolume *physPMT5 = new G4PVPlacement(meshRot, G4ThreeVector(-14.85*mm, 29.7*mm, 3.5), logicPMT, "physPMT", logicWorld, false, 5, true);
	// G4VPhysicalVolume *physPMT6 = new G4PVPlacement(meshRot, G4ThreeVector(14.85*mm, 29.7*mm, 3.5), logicPMT, "physPMT", logicWorld, false, 6, true);
	
	// G4VSolid *solidTopSpacer = topSpacer->GetSolid();
	// G4LogicalVolume *logicTopSpacer = new G4LogicalVolume(solidTopSpacer, PTFE, "logicTopSpacer");
	// logicTopSpacer->SetVisAttributes(purpleVisAttributes);

	// G4VPhysicalVolume *physTopSpacer = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 32.*mm), logicTopSpacer, "physTopSpacer", logicWorld, false, 0, true);
	
	// G4VSolid *solidElectrodeSeparationRing = electrodeSeparationRing->GetSolid();
	// G4LogicalVolume *logicElectrodeSeparationRing = new G4LogicalVolume(solidElectrodeSeparationRing, StainlessSteel, "logicElectrodeSeparationRing");
	// logicElectrodeSeparationRing->SetVisAttributes(yellowVisAttributes);

	// G4VPhysicalVolume *physTopElectrodeSeparationRing = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 62.*mm), logicElectrodeSeparationRing, "physElectrodeSeparationRing", logicWorld, false, 0, true);

	// G4VSolid *solidElectrodeMesh = electrodeMesh->GetSolid();
	// G4LogicalVolume *logicElectrodeMesh = new G4LogicalVolume(solidElectrodeMesh, StainlessSteel, "logicElectrodeMesh");
	// logicElectrodeMesh->SetVisAttributes(whiteVisAttributes);
	
	// G4VPhysicalVolume *physElectrodeMesh0 = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 63.8*mm), logicElectrodeMesh, "physElectrodeMesh", logicWorld, false, 0, true);
	// // G4VPhysicalVolume *physElectrodeMesh1 = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 69.*mm), logicElectrodeMesh, "physElectrodeMesh", logicWorld, false, 1, true);
	// // G4VPhysicalVolume *physElectrodeMesh2 = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 128.8*mm), logicElectrodeMesh, "physElectrodeMesh", logicWorld, false, 2, true);
	// // G4VPhysicalVolume *physElectrodeMesh3 = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 147.*mm), logicElectrodeMesh, "physElectrodeMesh", logicWorld, false, 3, true);
	
	// G4VSolid *solidSteelRing = steelRing->GetSolid();
	// G4LogicalVolume *logicSteelRing = new G4LogicalVolume(solidSteelRing, StainlessSteel, "logicSteelRing");
	// logicSteelRing->SetVisAttributes(blueVisAttributes);
	
	// G4VPhysicalVolume *physSteelRing0 = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 62.*mm), logicSteelRing, "physSteelRing", logicWorld, false, 0, true);
	
	// G4VPhysicalVolume *physSteelRing1 = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 76.*mm), logicSteelRing, "physSteelRing", logicWorld, false, 1, true);
	
	// G4VSolid *solidPESheeth = PESheeth->GetSolid();
	// G4LogicalVolume *logicPESheeth = new G4LogicalVolume(solidPESheeth, HDPE, "logicPESheeth");
	// logicPESheeth->SetVisAttributes(HDPEVisAttributes);
	// G4VPhysicalVolume *physPESheeth = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 78.*mm), logicPESheeth, "physPESheeth", logicWorld, false, 0, true);
	
	// G4VPhysicalVolume *physBottomElectrodeSeparationRing = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 137.*mm), logicElectrodeSeparationRing, "physElectrodeSeparationRing", logicWorld, false, 1, true);

	// G4VPhysicalVolume *physSteelRing2 = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 136.*mm), logicSteelRing, "physSteelRing", logicWorld, false, 2, true);
	
	// G4VPhysicalVolume *physSteelRing3 = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 150.*mm), logicSteelRing, "physSteelRing", logicWorld, false, 3, true);
	
	// G4VSolid *solidBottomCap = bottomPMTCap->GetSolid();
	// G4LogicalVolume *logicBottomCap = new G4LogicalVolume(solidBottomCap, PTFE, "logicBottomCap");
	// logicBottomCap->SetVisAttributes(defaultVisAttributes);
		
	// G4VSolid *solidBottomPMTCap = bottomPMTCap->GetSolid();
	// G4LogicalVolume *logicBottomPMTCap = new G4LogicalVolume(solidBottomPMTCap, PTFE, "logicBottomPMTCap");
	// logicBottomPMTCap->SetVisAttributes(defaultVisAttributes);
	
	// G4VPhysicalVolume *physBottomPMTCap = new G4PVPlacement(meshRot270, G4ThreeVector(0., 0., 134.*mm), logicBottomPMTCap, "physBottomPMTCap", logicWorld, false, 0, true);
	
	// G4VSolid *solidBottomPMT = bottomPMT->GetSolid();
	// logicBottomPMT = new G4LogicalVolume(solidBottomPMT, PTFE, "logicInnerBottomPMT");
	// logicBottomPMT->SetVisAttributes(PMTVisAttributes);
	
	// G4VPhysicalVolume *physBottomPMT = new G4PVPlacement(meshRot, G4ThreeVector(0., 0., 151.*mm), logicBottomPMT, "physBottomPMT", logicWorld, false, 0, true);
	
	
	G4Tubs *solidLXe = new G4Tubs("solidLXe", 0.*mm, 46.5*mm, 28.*mm, 0.*deg, 360.*deg); //4213333
	logicLXe = new G4LogicalVolume(solidLXe, LXe, "logicLXe");
	logicLXe->SetVisAttributes(invisible); //purpleVisAttributes
	
	G4VPhysicalVolume *physLXe = new G4PVPlacement(meshRot, G4ThreeVector(0., -72.*mm, 0.), logicLXe, "physLXe", logicWorld, false, 0, true);
	
	fScoringVolume = logicLXe;

	G4Tubs *solidGXe = new G4Tubs("solidGXe", 0.*mm, 46.5*mm, 8.*mm, 0.*deg, 360.*deg); //46.4213333
	G4LogicalVolume *logicGXe = new G4LogicalVolume(solidGXe, GXe, "logicGXe");
	logicGXe->SetVisAttributes(purpleVisAttributes);
	G4VPhysicalVolume *physGXe = new G4PVPlacement(meshRot, G4ThreeVector(0., -36.*mm, 0.), logicGXe, "physGXe", logicWorld, false, 0, true);
	
	//constructing electric field
	electricField = new ElectricField(-10.); //placeholder electric field value of 100 V/cm for lXe
	equation = new G4EqMagElectricField(electricField);
	G4int nvar = 8;
	auto pStepper = new G4DormandPrince745(equation, nvar);
	
	auto liquidFieldManager = new G4FieldManager(electricField);
	liquidFieldManager->SetDetectorField(electricField);
	
	G4double minStep = 0.001*mm;
	auto pIntegrationDriver = new G4IntegrationDriver<G4DormandPrince745>(minStep, pStepper, nvar);

	gasElectricField = new ElectricField(-250.); //placeholder electric field value of 250 V/cm for gXe
	gasEquation = new G4EqMagElectricField(gasElectricField);
	auto gasFieldManager = new G4FieldManager(gasElectricField);
	gasFieldManager->SetDetectorField(gasElectricField);
	
	chordFinder = new G4ChordFinder(pIntegrationDriver);

	// auto stepLimiter = new G4StepLimiter();
	// auto ePmanager = G4Electron::Electron()->GetProcessManager();
	// ePmanager->AddDiscreteProcess(stepLimiter);
	logicLXe->SetUserLimits(new G4UserLimits(2.5*mm));

	G4Region* fastRegion = new G4Region("LXeFastRegion");
    fastRegion->AddRootLogicalVolume(logicLXe);
    auto* driftModel = new LXeElectronDriftModel("LXeDriftModel", fastRegion, &detector);


	liquidFieldManager->SetChordFinder(chordFinder);
	gasFieldManager->SetChordFinder(chordFinder);
	
	logicLXe->SetFieldManager(liquidFieldManager, true);
	logicPESheeth->SetFieldManager(liquidFieldManager, true);

	logicGXe->SetFieldManager(gasFieldManager, true);
	
	
	return physWorld;
}

void MyDetectorConstruction::ConstructSDandField()
{
	G4SDManager *sdManager = G4SDManager::GetSDMpointer();
	
	sensPMT = new MySensitiveDetector("sensPMT");
	sdManager->AddNewDetector(sensPMT);
}
