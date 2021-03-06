//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: B3DetectorConstruction.cc 71323 2013-06-13 16:54:23Z gcosmo $
//
/// \file B3DetectorConstruction.cc
/// \brief Implementation of the B3DetectorConstruction class

#include "B3DetectorConstruction.hh"

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSDoseDeposit.hh"
#include "G4VisAttributes.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "B3SensitiveDetector.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B3DetectorConstruction::B3DetectorConstruction()
: G4VUserDetectorConstruction(),
  fCheckOverlaps(true)
{
  // **Material definition**
  DefineMaterials();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B3DetectorConstruction::~B3DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B3DetectorConstruction::DefineMaterials()
{

  G4double a; // mass of a mole
  G4double z; // mean number of protons
  G4String name, symbol;

  // Definition of element Oxygen
  a=16.00*g/mole;
  G4Element*  elO = new G4Element(name="Oxygen", symbol="O", z=8., a);

  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$   task 1a.1
  a=28.09*g/mole;
  G4Element*  elSi = new G4Element(name="Silicon", symbol="Si", z=14., a);
  a=174.97*g/mole;
  G4Element* elLu = new G4Element(name="Lutetium", symbol="Lu", z=71., a);
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


  // Declare the material with its density and number of components
  G4Material* LSO;

  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$    task 1a.1
  G4double density = 7.4*g/cm3;
  LSO = new G4Material("Lu2SiO5",  //its name 
                       density,    //its density
                       3);         //number of components

  //Add Element for Material "Lu2SiO5" specifiyng the number of each element
  LSO->AddElement(elSi, 1);
  LSO->AddElement(elLu, 2);
  LSO->AddElement(elO , 5);  
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

  // Dump the Table of registered materials 
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B3DetectorConstruction::Construct()
{   // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();
  
  //Crystal parameters
  //
  G4double cryst_dX = 22*mm, cryst_dY = 4*mm, cryst_dZ = 3*mm;

  
   // **Retrieve Nist Materials** 
  G4Material* cryst_mat   = nist->FindOrBuildMaterial("Lu2SiO5");
   
  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //     
  // World
  //
  G4double world_sizeX = 12*cm;
  G4double world_sizeYZ  = 2*cm;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
  
  G4Box* solidWorld =    
    new G4Box("World",                       //its name
       0.5*world_sizeX, 0.5*world_sizeYZ, 0.5*world_sizeYZ);     //its size
      
  G4LogicalVolume* logicWorld =                         
    new G4LogicalVolume(solidWorld,          //its solid
                        world_mat,           //its material
                        "World");            //its name
                                   
  G4VPhysicalVolume* physWorld = 
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      logicWorld,            //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking
                     
  //     
  // Crystal
  //
  G4double pos_dX = 3.8*cm;
  
  G4Box* solidCryst =    
    new G4Box("crystal",                    //its name
	      0.5*cryst_dX, 0.5*cryst_dY, 0.5*cryst_dZ); //its size
      
  G4LogicalVolume* logicCryst =                         
    new G4LogicalVolume(solidCryst,            //its solid
                        cryst_mat,             //its material
                        "CrystalLV");         //its name
               
  
   //non-scoring crystals
  // G4Box* crystalSV =    
  //  new G4Box("cryst",                    //its name
  //	      0.5*cryst_dX, 0.5*cryst_dY, 0.5*cryst_dZ); //its size
      
  //G4LogicalVolume* crystalLV =                         
  //  new G4LogicalVolume(crystalSV,            //its solid
  //                      cryst_mat,             //its material
  //                      "crystLV");         //its name
  //array of positions
  G4int nb_cryst = 9;
  
  G4ThreeVector positions[9] = {
    G4ThreeVector(pos_dX,-cryst_dY,cryst_dZ),
    G4ThreeVector(pos_dX,0,cryst_dZ),
    G4ThreeVector(pos_dX,cryst_dY,cryst_dZ),
    G4ThreeVector(pos_dX,-cryst_dY,0),
    G4ThreeVector(pos_dX,0,0),
    G4ThreeVector(pos_dX,cryst_dY,0),
    G4ThreeVector(pos_dX,-cryst_dY,-cryst_dZ),
    G4ThreeVector(pos_dX,0,-cryst_dZ),
    G4ThreeVector(pos_dX,cryst_dY,-cryst_dZ)
  };
    

  for (G4int icrys = 0; icrys < nb_cryst; icrys++) {
    //set all the crystals as scoring volumes
       new G4PVPlacement(0,                       //no rotation
                    positions[icrys],         //position
                    logicCryst,                //its logical volume
                    "crystal",              //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    icrys,                       //copy number
		    checkOverlaps);          //overlaps checking
  }
  //always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B3DetectorConstruction::ConstructSDandField()
{
  //
  // Register some of the volumes as "sensitive" and decide the 
  // type of sensitivity that they have
  //
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);
  
  // declare crystal as a MultiFunctionalDetector scorer
  //  
  // Create a new scorer (G4MultiFunctionalDetector) and set its 
  // "capability" to G4PSEnergyDeposit (will score total energy deposit)
  G4MultiFunctionalDetector* cryst = new G4MultiFunctionalDetector("crystal");
  G4VPrimitiveScorer* primitiv1 = new G4PSEnergyDeposit("edep");
  cryst->RegisterPrimitive(primitiv1);
  // Attach the scorer to the logical volume
  SetSensitiveDetector("CrystalLV",cryst);
  
  return;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
